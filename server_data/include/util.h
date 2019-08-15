//
// Created by Administrator on 2019/8/13.
//

#ifndef MY_PRO_UTIL_H
#define MY_PRO_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "make_log.h"
#include "redis_op.h"
#define CONFIG_PATH         "./conf/OBO_server_data.conf"

#define  IP_LEN             18
#define  PORT_LEN           6

#define  DB_NAME_LEN        20
#define  SQL_LEN            1024
#define  DB_USERNAME_LEN    128
#define  DB_PASSWD_LEN      128

#define  LOG_MODULE_SERVER_DATA     "sever_data"
#define  LOG_PROC_PERSISTENT        "per"
#define  LOG_PROC_CACHE             "cache"

typedef struct db_config{
    char db_ip[IP_LEN];
    char db_port[PORT_LEN];
    char db_basename[DB_NAME_LEN];
    char db_usename[DB_USERNAME_LEN];
    char db_passwd[DB_PASSWD_LEN];

    char cache_ip[IP_LEN];
    char cache_port[PORT_LEN];
}db_config_t;

extern db_config_t g_db_config;

void config_init();
char *make_response_json(int ret,char *reason);
char *make_response_gethash_json(int ret,char *reason,RVALUES rvalues,int value_num);
char *make_response_geo_drivers_json(int ret,char *reason,RGEO rgeo_array,int geo_num);

#endif //MY_PRO_UTIL_H

