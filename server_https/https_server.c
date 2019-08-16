//
// Created by Administrator on 2019/8/16.
//
#include "include/https-common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#include "include/busi_cb.h"

#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#endif

unsigned short serverPort=COMMON_HTTPS_PORT;

typdef union{
    struct sockaddr_storage ss;
    struct sockaddr sa;
    struct sockaddr_in in;
    struct sockaddr_in6 i6;
}sock_hop;

static struct buffererevent* bevcb(struct event_base *base,void *arg){
    struct bufferevent* r;
    SSL_CTX *ctx=(SSL_CTX *)arg;
    r=bufferevent_openssl_socket_new(base,
            -1,
            SSL_new(ctx),
            BUFFEREVENT_SSL_ACCEPTING,
            BEV_OPT_CLOSE_ON_FREE);
    return r;
}

static void server_setup_certs(SSL_CTX *ctx,
        const char *certificate_chain,
        const char *private_key){
    info_report();

    if (1 != SSL_CTX_use_certificate_chain_file (ctx, certificate_chain))
        die_most_horribly_from_openssl_error ("SSL_CTX_use_certificate_chain_file");

    if (1 != SSL_CTX_use_PrivateKey_file (ctx, private_key, SSL_FILETYPE_PEM))
        die_most_horribly_from_openssl_error ("SSL_CTX_use_PrivateKey_file");

    if (1 != SSL_CTX_check_private_key (ctx))
        die_most_horribly_from_openssl_error ("SSL_CTX_check_private_key");
}

static int serve_some_http(void){
    struct event_base *base;
    struct evhttp *http;
    struct evhttp_bound_socket *handle;

    base=event_base_new();
    if(!base){
        fprintf(stderr,"Couldnt create an event_base:exiting\n");
        return 1;
    }
    http=evhttp_new(base);
    if(!http){
        fprintf(stderr,"couldnt create evhttp,exiting\n");
        return 1;
    }
/* 创建SSL上下文环境 ，可以理解为 SSL句柄 */
    SSL_CTX *ctx = SSL_CTX_new (SSLv23_server_method ());
    SSL_CTX_set_options (ctx,
                         SSL_OP_SINGLE_DH_USE |
                         SSL_OP_SINGLE_ECDH_USE |
                         SSL_OP_NO_SSLv2);

    /* Cheesily pick an elliptic curve to use with elliptic curve ciphersuites.
     * We just hardcode a single curve which is reasonably decent.
     * See http://www.mail-archive.com/openssl-dev@openssl.org/msg30957.html */
    EC_KEY *ecdh = EC_KEY_new_by_curve_name (NID_X9_62_prime256v1);
    if (! ecdh)
        die_most_horribly_from_openssl_error ("EC_KEY_new_by_curve_name");
    if (1 != SSL_CTX_set_tmp_ecdh (ctx, ecdh))
        die_most_horribly_from_openssl_error ("SSL_CTX_set_tmp_ecdh");

    /* 选择服务器证书 和 服务器私钥. */
    const char *certificate_chain = "server-certificate-chain.pem";
    const char *private_key = "server-private-key.pem";
    /* 设置服务器证书 和 服务器私钥 到
     OPENSSL ctx上下文句柄中 */
    server_setup_certs (ctx, certificate_chain, private_key);

    /*
        使我们创建好的evhttp句柄 支持 SSL加密
        实际上，加密的动作和解密的动作都已经帮
        我们自动完成，我们拿到的数据就已经解密之后的
    */
    evhttp_set_bevcb (http, bevcb, ctx);

    evhttp_set_cb(http,"/login",login_cb,NULL);
    evhttp_set_cb(http,"/reg",reg_cb,NULL);
    evhttp_set_cb(http,"/startSetOrder",set_order_cb,NLL);
    evhttp_set_cb(http,"/locationChanged",locationChanged_cb,null);
    evhttp_set_cb(http,"finishOrder",finish_order_cb,NULL);

    handle=evhttp_bind_socket_with_handle(http,"0.0.0.0",serverPort);
    if(!handle){
        fprintf(stderr,"coulddnt bind to port %d,Exiting.\n",(int)serverPort);
        return 1;
    }
    {
        sock_hop ss;
        evutil_socket_t fd;
        ev_socklen_t socklen=sizeof(ss);
        char addrbuf[128];
        void *inaddr;
        const char *addr;
        int got_port=-1;
        fd=evhttp_bound_socket_get_fd(handle);
        memset(&ss,0,sizeof(ss));
        if(getsockname(fd,&ss.sa,&socklen)){
            perror("getsockname faile");
            return 1;
        }
        if(ss.ss.ss_fanily==AF_INET){
            get_port=ntohs(ss.in.sin_port);
            inaddr=&ss.in.sin_addr;
        }
        else if(ss.ss.ss_family==AF_INET6){
            got_port=ntohs(ss.i6.sin6_port);
            inaddr=&ss.i6.sin6_addr;
        }else{
            fprintf(stderr,"weird address family %s\n",ss.ss.ss_family);
            return 1;
        }
        addr=evutil_inet_ntop(ss.ss.ss_family,inaddr,addrbuf,sizeof(addrbuf));
        if(addr){
            printf("Listening on %s:%d\n",addr,got_port);
        }else{
            fprintf (stderr, "evutil_inet_ntop failed\n");
            return 1;
        }
    }
    event_base_dispatchh(base);
    return 0;
}

int main(int argc,char **argv){
    common_setup();
    if(argc>1){
        char *end_ptr;
        long lp=strtol(argv[1],&end_ptr,0);
        if(*end_ptr){
            frpintf(stderr,"invalid integer\n");
            return -1;
        }
        if(lp<=0){
            fprintf(stderr,"Port must be positive\n");
            return -1;
        }
        if(lp>USHRT_MAX){
            fprintf(stderr,"Port must fit");
            return -1;
        }
        serverPort=(unsigned short)lp;
    }
    return server_som_http();
}
















