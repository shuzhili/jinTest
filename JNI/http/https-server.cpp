//
// Created by Administrator on 2019/8/8.
//
#include "https-common.h"

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

#include <cJSON.h>

#define MYHTTPD_SIGNATURE   "MoCarHttpd v0.1"


#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#endif

unsigned short serverPort=COMMON_HTTPS_PORT;
typedef union{
    struct sockaddr_storage ss;
    struct sockaddr sa;
    struct sockaddr_in in;
    struct sockaddr_in6 i6;
}sock_hop;
static void login_cb(struct evhttp_request *req,void *arg){
    struct evbuffer *evb=NULL;
    const char *uri=evhttp_request_get_uri(req);
    struct evhttp_uri *decoded=NULL;

    if(evhtp_request_get_command(req)==EVHTTP_REQ_GET){
        struct evbuffer *buf=evbuffer_new();
        if(buf==NULL){return;}
        evbuffer_add_printf(buf,"Requested:%s\n",uri);
        evhttp_send_reply(req,HTTP_OK,"OK",buf);
        return;
    }
    if(evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req,200,"OK",NULL);
        return;
    }
    printf("Got a post request for<%s>\n",uri);
    decoded=evhttp_uri_parse(uri);
    if(!decoded){
        printf("it.s not a good uri,sending badrequest\n");
        evhttp_send_error(req,HTTP_BADREQUEST,0);
        return;
    }
    struct evbuffer *buf=evhttp_request_get_input_buffer(req);
    evbuffer_add(buf,"",1);
    char *payload=(char *)evbuffer_pullup(buf,-1);
    int post_data_len=evbuffer_get_length(buf,-1);
    char request_data_buffer[4096]={0};
    memcpy(request_data_buf,payload,post_data_len);

    printf("[post_data][%d]=\n %s\n",post_data_len,payload);
    //unpack json
    cJSON* root = cJSON_Parse(request_data_buf);
    cJSON* username = cJSON_GetObjectItem(root, "username");
    cJSON* password = cJSON_GetObjectItem(root, "password");
    cJSON* driver = cJSON_GetObjectItem(root, "driver");

    printf("username = %s\n", username->valuestring);
    printf("password = %s\n", password->valuestring);
    printf("driver = %s\n", driver->valuestring);

    cJSON_Delete(root);



    //packet json
    root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "result", "ok");
    //cJSON_AddStringToObject(root, "sessionid", "xxxxxxxx");

    char *response_data = cJSON_Print(root);
    cJSON_Delete(root);




    /* This holds the content we're sending. */

    //HTTP header
    evhttp_add_header(evhttp_request_get_output_headers(req), "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(evhttp_request_get_output_headers(req), "Connection", "close");

    evb = evbuffer_new ();
    evbuffer_add_printf(evb, "%s", response_data);
    //将封装好的evbuffer 发送给客户端
    evhttp_send_reply(req, HTTP_OK, "OK", evb);

    if (decoded)
        evhttp_uri_free (decoded);
    if (evb)
        evbuffer_free (evb);


    printf("[response]:\n");
    printf("%s\n", response_data);

    free(response_data);
}
static struct bufferevent* bevch(struct event_base *base,void *arg){
    struct bufferevent* r;
    SSL_CTX *ctx=(SSL_CTX*)arg;
    r=bufferevent_openssl_socket_new(
            base,-1,SSL_new(ctx),
            BUFFEREVENT_SSL_ACCEPTING,
            BEV_OPT_CLOSE_ON_FREE);
    return r;
}
static void server_setup_certs (SSL_CTX *ctx,
                                const char *certificate_chain,
                                const char *private_key)
{
    info_report ("Loading certificate chain from '%s'\n"
                 "and private key from '%s'\n",
                 certificate_chain, private_key);

    if (1 != SSL_CTX_use_certificate_chain_file (ctx, certificate_chain))
        die_most_horribly_from_openssl_error ("SSL_CTX_use_certificate_chain_file");

    if (1 != SSL_CTX_use_PrivateKey_file (ctx, private_key, SSL_FILETYPE_PEM))
        die_most_horribly_from_openssl_error ("SSL_CTX_use_PrivateKey_file");

    if (1 != SSL_CTX_check_private_key (ctx))
        die_most_horribly_from_openssl_error ("SSL_CTX_check_private_key");
}
static int server_some_http(void){
    struct event_base *base;
    struct evhttp *http;
    struct evhttp_bound_socket *handle;

    base=event_base_new();
    if(!base){
        fprintf(stderr,"Couldn't create an event_base: exiting\\n");
        return 1;
    }
    http=evhttp_new(base);
    if(!http){
        fprintf(stderr,"couldn't create evhttp. Exiting.\n");
        return 1;
    }
    SSL_CTX *ctx=SSL_CTX_new(SSLv23_server_method());
    SSL_CTX_set_options(ctx,
            SSL_OP_SINGLE_DH_USE|
            SSL_OP_SINGLE_ECDH_USE|
            SSL_OP_NO_SSLv2);
    EC_KEY *ecdh=EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if(!ecdh){
        die_most_horribly_from_openssl_err("EC_KEY_new_by_curve_name");
    }
    if(1!=SSL_CTX_set_tmp_ecdh(ctx,ecdh)){
        die_most_horribly_form_openssl_error("SSL_CTX_set_tmp_ecdh");
    }
    const char *certificate_chain="server-certificate-chain.pem";
    const char *private_key="server-private-key.pem";
    server_setup_certs(ctx,certificate_chain,private_key);

    evhttp_set_bevcb(http,bevcb,ctx);

    evhttp_set_cb(http,"/login",login_cb,NULL);
    handle=evhttp_bind_socket_with_handle(http,"0.0.0.0",serverPort);
    if(!handle){
        fprintf(stderr,"couldn't bind to port %d",serverPort);
        return 1;
    }
    {
        sock_hop ss;
        evutil_socket_t fd;
        ev_socklen_t socklen= sizeof(ss);
        char addrbuf[128];
        void *inaddr;
        int got_port=-1;

        fd=evhttp_bound_socket_get_fd(handle);
        menset(&ss,0, sizeof(ss));
        if(getsockname(fd,&ss.sa,&socklen)){
            perror("getsockname() failed");
            return 1;
        }
        if(ss.ss.ss_family==AF_INET){
            got_port=ntohs(ss.in.sin_port);
            inaddr=&ss.in.sin_addr;
        }else if(ss.ss.ss_family==AF_INET6){
            got_port=ntohs(ss.i6.sin6_port);
            inaddr=&ss.i6.sin6_addr;
        }else{
            fprintf(stderr,"weird address family %s\n",ss.ss.ss_family);
            return 1;
        }
        addr=evutil_inet_ntop(ss.ss.ss_family,inaddr,
                addrbuf,sizeof(addrbuf));
        if(addr){
            printf("listening on %s:%d\n",addr,got_port);
        }else{
            fprintf(stderr,"evutil_inet_ntop failed\n");
            return 1;
        }
    }
    event_base_dispath(base);
    return 0;
}
int main(int argc,char **argv){
    common_setup();
    if(argc>1){
        char *end_ptr;
        long lp=strtol(argv[1],&end_ptr,0);
        if(*end_ptr){
            fprintf(stderr,"Invalid integer\n");
            return -1;
        }
        if(lp<=0){
            fprintf(stderr,"Port must be positive\n");
            return -1;
        }
        if(lp>=USHRT_MAX){
            fprintf(stderr,"Port must fit 16-bid range\n");
            return -1;
        }
        severPort=(unsigned short)lp;
    }
    return server_some_http();
}
