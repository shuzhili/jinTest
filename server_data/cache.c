//
// Created by Administrator on 2019/8/13.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

#include "include/https-common.h"
#include "include/busi_cb.h"
#include "include/util.h"
#include "include/make_log.h"

void cache_store_cb(struct evhttp_request *req,void *arg){
    struct evbuffer *evb=NULL;
    const char *uri=evhttp_request_get_uri(req);
    struct evhttp_uri *decoded=NULL;
    if(evhttp_request_get_command(req)==EVHTTP_REQ_GET){
        struct evbuffer *buf=evbuffer_new();
        if(buf==NULL)return;
        evbuffer_add_printf(buf,"Requested:%s\n",uri);
        evhttp_send_reply(req,HTTP_OK,"OK",buf);
        printf("get uri:%s\n",uri);
        LOG(LOG_MODULE_SERVER_DATA,LOG_PROC_CACHE,"get uri:%s\n",uri);
        return;
    }
    if(evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req,200,"OK",NULL);
        return;
    }
    LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_CACHE, "Got a POST request for <%s>\n", uri);
    decode=evhttp_uri_parse(uri);
    if(!decoded){
        LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_CACHE, "It's not a good URI. Sending BADREQUEST\n");
        evhttp_send_error(req,HTTP_BADREQUEST,0);
        return;
    }

    struct evbuffer *buf=evhttp_request_get_input_buffer(req);
    evbbufer_add(buf,"",1);
    char *payload=(char*)evbuffer_pullup(buf,-1);
    int post_data_len=evbuffer_get_length(buf);
    char request_data_buf[4096]={0};
    mencpy(request_data_buf,payload,post_data_len);
    LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_CACHE, "[post_data][%d]=\n %s\n", post_data_len, payload);
    /*
       具体的：可以根据Post的参数执行相应操作，然后将结果输出
       ...
     */
    //=========================================

    char *response_data=NULL;
    response_data=deal_cache(request_data_buf);
    evhttp_add_header(evhttp_request_get_out_put_headers(req),"Server",MYHTTPD_SIGNATURE);
    evhttp_add_header(evhttp_request_get_out_put_headers(req),"Content-Type","text/plain;charset=UTF-8");
    evhttp_add_header(evhttp_request_get_out_put_headers(req),"Connection","close");

    evb=evbuffer_new();
    evbuffer_add_printf(evb,"%s",response_data);
    evhttp_send_reply(req,HTTP_OK,"OK",evb);

    if(decode){
        evhttp_uri_free(decoded);
    }
    if(evb){
        evbuffer_free(evb);
    }
    if(response_data!=NULL){
        LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_CACHE, "[response]:\n");
        LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_CACHE, "%s\n", response_data);

        free(response_data);
    }
}


















