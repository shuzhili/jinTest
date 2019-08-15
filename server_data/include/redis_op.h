//
// Created by Administrator on 2019/8/14.
//
#ifndef MY_PRO_REDIS_OP_H
#define MY_PRO_REDIS_OP_H

#include "hiredis.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "make_log.h"

#define REDIS_LOG_MODULE        "database"
#define REDIS_LOG_PROC          "redis"
#define REDIS_COMMAND_SIZE      300
#define FIELD_ID_SIZE          300
#define VALUE_ID_SIZE           1024
typedef char (*RCOMMANDS)[REDIS_COMMAND_SIZE];
typedef char (*RFIELDS)[FIELD_ID_SIZE];
typedef char (*RVALUES)[VALUE_ID_SIZE];

typedef struct get_location{
    char name[VALUE_ID_SIZE];
    char distance[VALUE_ID_SIZE];
    char longitude[VALUE_ID_SIZE];
    char latitude[VALUE_ID_SIZE];
}geo_location_t;
#define GET_SIZE sizeof(geo_location_t)
typedef geo_location_t (*RGEO);

redisContext* rop_connectdb_nopwd(char  *ip_str,char *port_str);
redisContext* rop_connectdb(char *ip_str,char *port_str,char *pwd);
redisContext* rop_connectdb_unix(char *sock_path, char *pwd);
redisContext* rop_connect_timeout(char *ip_str,char *port_str,struct timeval *timeout);

void rop_disconnect(redisContext* context);
int rop_select_database(redisContext* context,unsigned int db_num);
int rop_flush_database(redisContext* context);
int rop_is_key_exit(redistContext* context,char *key);
int rop_delete_key(redisContext* conn,char *key);
void rop_show_keys(redisContext* conn,char *patten);
int rop_set_key_lifecycle(redisContext* conn,char *key,time_t delete_time);
int rop_set_key_lifetime(redisContext* conn,char *key,int second);
int rop_create_or_replace_hash_table(redisContext* conn,char *key, unsigned int element_num,
                                        RFIELDS fields,RVALUES rvalues);
int rop_hiincrement_one_field(redisContext *conn,char *key,char *field,unsigned int num);
int rop_hash_set_append(redisContext *conn,char *key,RFIELDS fields,RVALUES values,int val_num);
int rop_hash_get_append(redisContext *conn,char *key,RFIELDS fields,RVALUES values,int val_num);
int rop_hash_set(redisContext *conn,char *key,char *field,char *value);
int rop_zset_increment(redisContext *conn,char *key,char *member);
int rop_zset_get_score(redisContext *conn,char *key,char *member);
int rop_zset_rem_member(redisContext *conn,char *key,char *member);
int rop_zset_increment_append(redisContext *conn,char *key,RVALUES values,int val_num);
int rop_list_push_append(redisContext *conn,char *key,RVALUES values,int val_num);
int rop_list_push(redisContext *conn,char *key,char *value);
int rop_get_list_cnt(redisContext *conn,char *key);
int rop_trim_list(redisConext *conn,char *key,int begin,int end);
int rop_range_list(redisContext *conn,char *key,int from_pos,int count,RVALUES values,int *get_num);
int rop_redis_append(redisContext *conn,RCOMMANDS cmds,int cmd_num);
int rop_redis_command(redisContext *conn,char *cmd);
void rop_text_reply_type(redisReply *reply);
int rop_set_string(redisContext *conn,char *key,char *value);
int rop_get_string(redisContext *conn,char *key,char *value);
int rop_increment_string(redisContext *conn,char *key,int *get_num);
int rop_geo_radius(redisContext *conn,const char *key,const char *longitude,const char *latitude,
        const char *radius,RGEO *get_array_p,int *get_num);
int rop_add_geo(redisContext *conn,const char *key,const char *menber,const char *longitude,const char *latitude);

#endif //MY_PRO_REDIS_OP_H