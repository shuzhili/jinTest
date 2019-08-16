//
// Created by Administrator on 2019/8/16.
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

#include <cJSON.h>
#include <curl/curl.h>
#include "include/https-common.h"
#include "include/busi_cb.h"
#include "include/util.h"

void reg_cb(struct evhttp_request *req,void *arg){
    int ret=0;
    struct evbuffer *evb=NULL;
    const char *uri=evhttp_request_get_uri(req);
    struct evhttp_uri *decoded=NULL;

    if(evhttp_request_get_command(req)==EVHTTP_REQ_GET){
        struct evbuffer *buf=evbuffer_new();
        if(buf==NULL){return;}
        evbuffer_add_printf(buf,"Requested：%s\n“,uri);
        evhttp_send_reply(req,HTTP_OK,"OK",buf);
        printf("get uri:%s\n",uri);
        LOG(LOG_MODULE,LOG_PROC_REG,"get uri:%s\n",uri);
        return;
    }
    if(evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhtt_send_reply(req,200,"OK",NULL);
        retrun;
    }
    printf("post request <%s>\n",uri);
    decoded=evhttp_uri_parse(uri);
    if(!decoded){
        printf("badrequest");
        evhttp_send_error(req,HTTP_BADREQUEST,0);
        return;
    }
    /* Decode the payload */
    struct evbuffer *buf = evhttp_request_get_input_buffer (req);
    evbuffer_add (buf, "", 1);    /* NUL-terminate the buffer */
    char *payload = (char *) evbuffer_pullup (buf, -1);
    int post_data_len = evbuffer_get_length(buf);
    char request_data_buf[4096] = {0};
    memcpy(request_data_buf, payload, post_data_len);
    printf("[post_data][%d]=\n %s\n", post_data_len, payload);
    LOG(LOG_MODULE, LOG_PROC_REG, "[post_data][%d]=\n %s\n", post_data_len, payload);

    cJSON* root=cJSON_Parse(request_data_buf);
    cJSON* username=cJSON_GetObejctItem(root,"username");
    cJSON* password=cJSON_GetObjectItem(root,"password");
    cJSON* isDriver=cJSON_GetObjectItem(root,"driver");
    cJSON* tel=cJSON_GetObjectItem(root,"tel");
    cJSON* email=cJSON_GetObjectItem(root,"email");
    cJSON* id_card=cJSON_GetObjectItem(root,"id_card");

    ret=curl_to_dataserver_req(username->valuestring,
                                password->valuestring,
                                tel->valuestring,
                                email->valuestring,
                                isDriver->valuestring,
                                id_card->valuestring);
    char sessionid[SESSIONID_STR_LEN]={0};
    char *recode=RECODE_OK;
    if(ret==0){
        create_sessionid(isDriver->valuestring,sessionid);
        ret=curl_to_cacheserver_session(username->valuestring,sessionid,ORDER_ID_NONE);
    }
    if(ret==0){
        ret=curl_to_cacheserver_lifecycle(sessionid,SESSIONID_LIFECYCLE);
    }
    char *response_data=make_reg_login_res_json(ret,recode,sessionid,"req error");
    cJSON_Delete(root);

    //HTTP header
    evhttp_add_header(evhttp_request_get_output_headers(req), "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(evhttp_request_get_output_headers(req), "Connection", "close");

    evb=evbuffer_new();
    evbuffer_add_printf(evb,"%s",response_data);
    evhttp_send_reply(req,HTTP_OK,"OK",evb);

    if (decoded)
        evhttp_uri_free (decoded);
    if (evb)
        evbuffer_free (evb);


    printf("[response]:\n");
    printf("%s\n", response_data);
    LOG(LOG_MODULE, LOG_PROC_REG, "[response]:\n");
    LOG(LOG_MODULE, LOG_PROC_REG, "%s\n", response_data);

    free(response_data);
};