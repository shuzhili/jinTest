//
// Created by Administrator on 2019/8/15.
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
#include <curl/curl.h>
#include "include/https-common.h"
#include "include/busi_cb.h"
#include "include/util.h"

void login_cb(struct evhttp_request *req,void *arg){
    int ret=0;
    struct evbuffer *evb=NULL;
    const char *uri=evhttp_request_get_uri(req);
    struct evhttp_uri *decoded=NULL;

    if(evhttp_request_get_command(req)==EVHTTP_REQ_GET){
        struct evbuffer *buf=evbuffer_new();
        if(buf==NULL){return;}
        evbuffer_add_printf(buf,"Requested:%s\n",uri);
        evhttp_send_reply(req,HTTP_OK,"OK",buf);
        printf("get uri:%s\n",uri);
        LOG(LOG_MODULE,LOG_PROC_LOGINN,"get uri:%s",uri);
    }
    if(evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req,200,"OK",NULL);
        return;
    }
    printf("Got a POST request for<%s>\n",uri);
    LOG(LOG_MODULE,LOG_PROC_LOGIN,"get uri:%s",uri);

    decoded=evhttp_uri_parse(uri);
    if(!decoded){
        printf("Its not a good URI,sending badrequest");
        evhttp_send_serror(req,HTTP_BADREQUEST,0);
        return;
    }
    struct evbuffer *buf=evhttp_request_get_input_buffer(req);
    evbuffer_add(buf,"",1);
    char *payload=(char*)evbuffer_pullup(buf,-1);
    int post_data_len=evbuffer_get_length(buf);
    memcpy(request_data_buf,payload,post_data_len);

    cJSON* root=cJSON_Parse(rquest_data_buf);
    cJSON* username=cJSON_GetObjectItem(root,"username");
    cJSON* password=cJSON_GetObjectItem(root,"password");
    cJSON* isDriver=cJSON_GetObjectItem(root,"driver");

    printf("username=%s\n",username->valuestring);

    ret=curl_to_dataserver_login(username->valuestring,password->valuestring,isDriver->valuestring);
    char sesionid[SESSIONID_STR_LEN]={0};
    if(ret==0){
        create_sessionid(isDriver->valuestring,sesionid);
        ret=curl_to_cacheserver_session(username->valuestring,sesionid,ORDER_ID_NONE);
    }
    if(ret==0){
        ret=curl_to_cacheserver_lifecycle(sesionid,SESSIONID_LIFECYCLE);
    }
    char *response_data=make_reg_login_res_json(ret,recode,sesionid,"login error");
    cJSON_Delete(root);

    //HTTP header
    evhttp_add_header(evhttp_request_get_output_headers(req), "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(evhttp_request_get_output_headers(req), "Connection", "close");

    evb=evbuffer_new();
    evbuffer_add_printf(evb,"%s",response_data);
    evhttp_send_reply(req,HTTP_OK,"OK",evb);

    if(decoded){
        evhttp_uri_free(decoded);
    }
    if(evb){
        evbfuffer_free(evb);
    }
    printf("[response]:\n");
    printf("%s\n",response_data);
    LOG(LOG_MODULE,LOG_PROC_LOGIN,"[response]:\n");
    free(response_data);
}




























