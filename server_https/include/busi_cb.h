//
// Created by Administrator on 2019/8/15.
//

#ifndef MY_PRO_BUSI_CB_H
#define MY_PRO_BUSI_CB_H

#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalg_struct.h>

#define MYHTTPD_SIGNATURE   "OBOHttpd v0.1"

#define TIME_STR_LEN        (32)
#define UUID_STR_LEN        (36)
#define SESSIONID_STR_LEN   (64)
#define ORDERID_STR_LEN     (64)
#define DISTANCE_STR_LEN    (20)
#define LOCATION_POINT_STR_LEN  (20)
#define ADDRESS_STR_LEN     (256)
#define RESPONSE_DATA_LEN   (4096)
#define USERNAME_STR_LEN    (256)

#define URI_DATA_SERVER_PER "https://101.200.190.150:18889/persistent"
#define URI_DATA_SERVER_CHE "https://101.200.190.150:18889/cache"

#define SESSIONID_LIFECYCLE (600)
#define ORDER_ID_NONE       "NONE"

#define KEY_ONNLINE_DRIVERS_GEO_ZSET    "ONLINE_DRIVER_GEO_ZSET"
#define GEO_RADIUS                      "200"

#define RECODE_OK                       "0"
#define RECODE_SESSION_ERROR            "1"
#define RECODE_SERVER_ERROR             "2"
#define RECODE_NO_DRIVER                "3"

#define STATUS_DRIVER_IDLE              "idel"
#define STATUS_DRIVER_CATCH              "catching"
#define STATUS_DRIVER_READY              "ready"
#define STATUS_DRIVER_DRIVE              "driving"
#define STATUS_PASSENGER_IDLE           "idle"
#define STATUS_PASSENGER_WAIT           "waiting"
#define STATUS_PASSENGER_TRAVEL           "traveling"

#define LOG_MODULE                      "OBO_BUSI"
#define LOG_PROC_LOGIN                  "login"
#define LOG_PROC_REG                    "login"
#define LOG_PROC_SETORDER               "set_order"
#define LOG_PROC_FINISHORDER            "finish_order"
#define LOG_PROC_REMOTE_CURL            "remote_curl"

typedef struct curl_response_data{
    char data[RESPONSE_DATA_LEN];
    int data_len;
}curl_response_data_t;

void get_time_str(char* time_str);
char* get_random_uuid(char *str);
char* create_sessionid(const char *isDriver,char *sessionid);
char* create_orderid(char *orderid);

//返回前端json数据包封装
char *make_reg_login_res_json(int ret, char* recode, char *sessionid, char *reason);
char *make_gen_res_json(int ret, char* recode, char *reason);
char *make_driver_locationChanged_res_json(int ret, char *recode, char* status, char *orderid, char *reason, char *ptemp_longitude, char *ptemp_latitude);
char *make_passenger_locationChanged_res_json(int ret, char *recode, char* status, char *orderid, char *reason, char *dtemp_longitude, char *dtemp_latitude, char *order_status);

typedef struct geo_driver{
    char sessionid[SESSIONID_STR_LEN];
    char distance[DISTANCE_STR_LEN];
    char longitude[LOCATION_POINT_STR_LEN];
    char latitude[LOCATION_POINT_STR_LEN];
}geo_driver_t;

typedef struct order{
    char orderid[ORDERID_STR_LEN];

    char passenger_username[ORDERID_STR_LEN];
    char driver_username[ORDERID_STR_LEN];

    char create_order_time[TIME_STR_LEN];
    char start_order_time[TIME_STR_LEN];
    char end_time[TIME_STR_LEN];

    char src_address[ADDRESS_STR_LEN];
    char dst_address[ADDRESS_STR_LEN];
    char src_longitude[LOCATION_POINT_STR_LEN];
    char src_latitude[LOCATION_POINT_STR_LEN];
    char dst_longitude[LOCATION_POINT_STR_LEN];
    char dst_latitude[LOCATION_POINT_STR_LEN];

    char src_address_real[ADDRESS_STR_LEN];
    char dst_address_real[ADDRESS_STR_LEN];
    char src_longitude_real[LOCATION_POINT_STR_LEN];
    char src_latitude_real[LOCATION_POINT_STR_LEN];
    char dst_longitude_real[LOCATION_POINT_STR_LEN];
    char dst_latitude_real[LOCATION_POINT_STR_LEN];

    char RMB[64];
}order_t;

#define ORDER_INFO_LEN      sizeof(struct order)

void login_cb(struct evhttp_request *req,void *arg);
void req_cb(struct evhttp_request *req,void *arg);
void set_order_cb(struct evhttp_request *req,void *arg);
void locationChanged_cb(struct evhttp_request *req,void *arg);
void finish_order_cb(struct evhttp_request *req,void *arg);

int curl_to_dataserver_reg(const char* username,const char* password,const char* tel,
                            const char* email,const char* isDriver,const char* id_card);

int curl_to_dataserver_login(const char *username,const char *password,const char* isDriver);

int curl_to_dataserver_create_order(order_t *order);
int curl_to_cacheserver_radiusGeo(const char *key, const char *longitude, const char *latitude,const char *radius, geo_drvier_t **geo_drivers_p, int *driver_count);
int curl_to_cacheserver_session(const char *username,const char* sessionid,const char *orderid);
int curl_to_cacheserver_lifecycle(const char *sessionid,int lifecycle);
int curl_to_cacheserver_create_order(const char *orderid,
                                     const char *passenger_sessionid,
                                     const char *driver_sessionid,
                                     const char *create_order_time,
                                     const char *src_address,
                                     const char *dst_address,
                                     const char *src_longitude,
                                     const char *src_latitude,
                                     const char *dst_longitude,
                                     const char *dst_latitude,
                                     const char *RMB);
int curl_to_cacheserver_startorder_real(char *orderid,char *order_status,char *start_driver_time,
                                        char *src_address_real,char* src_longitude_real,char *src_latitude_real);
int curl_to_cacheserver_set_orderid(const char *sessionid,const char *orderid);
int curl_to_cacheserver_get_orderid(const char *sessionid,char *order_id);
int curl_to_cacheserver_existsessionid(const char *sessionid);
int curl_to_cacheserver_remGeo(const char *sessionid);
int curl_to_cacheserver_addGeo(const char* sessionid,const char *longitude,const char *latitude);
int curl_to_cacheserver_set_dtemp_location(const char *orderid,const char *dtemp_longitude,const char *dtemp_latitude);
int curl_to_cacheserver_set_ptemp_location(const char *orderid,const char *ptemp_longitude,const char *ptemp_latitude);
int curl_to_cacheserver_get_dtemp_location(const char *orderid, char* dtemp_longitude,char *dtemp_latitude);
int curl_to_cacheserver_get_ptemp_location(const char *ordreid,char* ptemp_longitude,char *ptemp_latitude);
int curl_to_cacheserver_get_orderd(const char *orderid,char *passenger_sessionid,char *driver_sessionid,order_t *order_info);
int curl_to_cacheserver_get_order_status(const char *orderid,char *order_status);
int curl_to_cacheserver_get_username(const char *sessionid,char *usrname);
int curl_to_cacheserver_delete_key(const char *key);
#endif //MY_PRO_BUSI_CB_H





























