//
// Created by Administrator on 2019/8/15.
//
/**
 * 持久数据库 主入口
 */
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

void persistent_store_cb(struct evhttp_request *req,void *arg){
    struct evbuffer *evb=NULL;
    const char *uri=evhttp_request_get_url(req);
    struct evhttp_uri *decoded=NULL;

    if(evhttp_request_get_command(req)==EVHTTP_REQ_GET){
        struct evbuffer *buf=evbuffer_new();
        if(buf==NULL)return;
        evbuffer_add_printf(buf,"Requested:%s\n",uri);
        evhttp_send_reply(req,HTTP_OK,"OK",buf);
        LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_PERSISTENT, "get uri:%s\n", uri);
        return;
    }
    if(evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req,200,"OK",NULL);
        return;
    }
    LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_PERSISTENT, "Got a POST request for <%s>\n", uri);
    decoded=evhttp_uri_parse(uri);
    if(!decoded){
        LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_PERSISTENT, "It's not a good URI. Sending BADREQUEST\n");
        evhttp_send_error (req, HTTP_BADREQUEST, 0);
        return;
    }

    struct evbuffer *buf = evhttp_request_get_input_buffer (req);
    evbuffer_add (buf, "", 1);    /* NUL-terminate the buffer */
    char *payload = (char *) evbuffer_pullup (buf, -1);
    int post_data_len = evbuffer_get_length(buf);
    char request_data_buf[4096] = {0};
    memcpy(request_data_buf, payload, post_data_len);
    LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_PERSISTENT, "[post_data][%d]=\n %s\n", post_data_len, payload);

    char* response_data=NULL;
    //处理持久化数据
    response_data=deal_persistent(request_data_buf);

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
    if (response_data != NULL) {
        LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_PERSISTENT, "[response]:\n");
        LOG(LOG_MODULE_SERVER_DATA, LOG_PROC_PERSISTENT, "%s\n", response_data);

        free(response_data);
    }

}
























