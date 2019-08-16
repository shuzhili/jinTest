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

#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#include <curl/curl.h>
#include "include/cJSON.h"
#include "include/https-common.h"
#include "include/busi_cb.h"
#include "include/util.h"

void locationChanged_cb(struct evhttp_request *req, void *arg) {
    int ret = 0;
    struct evbuffer *evb = NULL;
    const char *uri = evhttp_request_get_uri(req);
    struct evhttp_uri *decoded = NULL;

    /* 判断 req 是否是GET 请求 */
    if (evhttp_request_get_command(req) == EVHTTP_REQ_GET) {
        struct evbuffer *buf = evbuffer_new();
        if (buf == NULL) return;
        evbuffer_add_printf(buf, "Requested: %s\n", uri);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        printf("get uri:%s\n", uri);
        return;
    }

    /* 这里只处理Post请求, Get请求，就直接return 200 OK  */
    if (evhttp_request_get_command(req) != EVHTTP_REQ_POST) {
        evhttp_send_reply(req, 200, "OK", NULL);
        return;
    }

    printf("Got a POST request for <%s>\n", uri);

    //判断此URI是否合法
    decoded = evhttp_uri_parse(uri);
    if (!decoded) {
        printf("It's not a good URI. Sending BADREQUEST\n");
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }

    /* Decode the payload */
    struct evbuffer *buf = evhttp_request_get_input_buffer(req);
    evbuffer_add(buf, "", 1);    /* NUL-terminate the buffer */
    char *payload = (char *) evbuffer_pullup(buf, -1);
    int post_data_len = evbuffer_get_length(buf);
    char request_data_buf[4096] = {0};
    memcpy(request_data_buf, payload, post_data_len);
    printf("[post_data][%d]=\n %s\n", post_data_len, payload);

    char *response_data = NULL;
    cJSON *root = cJSON_Parse(request_data_buf);
    cJSON *sessionid = cJSON_GetObjectItem(root, "sessionid");
    cJSON *driver = cJSON_GetObjectItem(root, "driver");
    cJSON *status = cJSON_GetObjectItem(root, "status");
    cJSON *longitude = cJSON_GetObjectItem(root, "longitude");
    cJSON *latitude = cJSON_GetObjectItem(root, "latitude");
    cJSON *address = cJSON_GetObjectItem(root, "address");

    if (strcmp(driver->valuestring, "no") == 0 &&
    strcmp(status->valuestring,OBO_STATIS_PASSNGER_WAIT)==0) {
        cJSON* dst_longitude=cJSON_GetObjectItem(root,"dst_longitude");
        cJSON* dst_latitude=cJSON_GetObjectItem(root,"dst_latitude");
        cJSON* dst_address=cJSON_GetObjectItem(root,"dst_address");
    }

    char orderid[ORDERID_STR_LEN]={0};
    char* recode=RECODE_OK;

    ret=curl_to_cacheserver_existsessionid(sessionid->valuestring);
    if(ret!=0){
        recode=RECODE_SERVER_ERROR;
    }
    if(ret==0){
        ret=curl_to_cacheserver_lifecycle(sessionid->valuestring,SESSIONID_LIFECYCLE);
    }
    if(ret==0){
        ret=curl_to_cacheserver_get_orderid(sessionid->valuestring,orderid);
        printf("orderid=%s\n",orderid);
    }
    if(ret==0){
        //===========司机分支==============//
        if(strcmp(driver->valuestring,"yes")==0){
            cJSON* autosend=cJSON_GetObjectItem(root,"autosend");
            printf("autosend=%s\n",autosend->valuestring);

            char ptemp_longitude[LOCATION_POINT_STR_LEN]={0};
            char ptemp_latitude[LOCATION_POINT_STR_LEN]={0};

            if(strcmp(status->valuestring,STATUS_DRIVER_DRIVE)==0){
                curl_to_cacheserver_remGeo(sessionid->valuestring);
                if(strcmp(autosend->valuestring,"yes")==0){
                    char now_time[TIME_STR_LEN]={0};
                    get_time_str(now_time);
                    curl_to_cacheserver_startorder_real(orderid,now_time,address->valuestring,longitude->valuestring,latitude->valuestring);
                }
                curl_to_cacheserver_set_dtemp_location(orderid,longitude->valuestring,latitude->valuestring);
                curl_to_cacheserver_get_ptemp_location(orderid,ptemp_longitude,ptemp_latitude);
            }else if(strcmp(status->valuestring,STATUS_DRIVER_IDLE)==0){
                ret=curl_to_cacheserver_remGeo(sessionid->valuestring);
            }else if(strcmp(status->valuestring,STATUS_DRIVER_CATCH)==0){
                curl_to_cacheserver_remGeo(sessionid->valuestring);
                curl_to_cacheserver_set_dtemp_location(orderid,longitude->valuestring,latitude->valuestring);
                curl_to_cacheserver_get_ptemp_location(orderid,ptemp_longitude,ptemp_latitude);
            }else if(strcmp(status->valuestring,STATUS_DRIVER_READY)==0){
                curl_to_cacheserver_remGeo(sessionid->valuestring);
                curl_to_cacheserver_addGeo(sessionid->valuestring,longitude->valuestring,latitude->valuestring);
            }
            response_data=make_driver_locationChanged_res_json(ret,recode,status->valuestring,orderid,"Driver LocationChanged error", ptemp_longitude, ptemp_latitude);
            goto END;
        }
        //=============乘客分支==============//
        else if(strcmp(driver->valuestring,"no")==0){
            char dtemp_longitude[LOCATION_POINT_STR_LEN]={0};
            char dtemp_latitude[LOCATION_POINT_STR_LEN]={0};

            if(strcmp(status->valuestring,STATUS_PASSENGER_TRAVEL)==0){
                curl_to_cacheserver_set_ptemp_location(orderid,longitude->valuestring,latitude->valuestring);
                curl_to_cacheserver_get_dtemp_location(orderid,dtemp_longitude,dtemp_latitude);
            }else if(strcmp(status->valuestring,STATUS_PASSENGER_IDLE)==0){

            }else if(strcmp(status->valuestring,STATUS_PASSENGER_WAIT)){
                curl_to_cacheserver_set_ptemp_location(orderid,longitude->valuestring,latitude->valuestring);
                curl_to_cacheserver_get_dtemp_location(orderid,dtemp_longitude,dtemp_latitude);
                curl_to_cacheserver_get_order_status(orderid,order_status);
            }
            response_data=make_passenger_locationChanged_res_json(ret,recode,status->valuestring,orderid,"Passenger LocationChanged error", dtemp_longitude, dtemp_latitude, order_status);
            goto END;
        }
    }
    response_data=make_get_res_json(ret,recode,"loactionchanged error");

END:
   cJSON_Delete(root);

    // =========================================================

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
























