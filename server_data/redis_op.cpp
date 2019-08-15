//
// Created by Administrator on 2019/8/14.
//

#include "include/redis_op.h"

static char *
make_hmset_command(char *key, unsigned int element_num, RFIELDS fields, RVALUES rvalues) {
    char *cmd = NULL;
    usigned int buf_size = 0;
    unsigned int use_size = 0;
    unsigned int i = 0;
    cmd = (char *) malloc(1024 * 1024);
    if (cmd == NULL) {
        goto END;
    }
    memset(cmd, 0, 1024 * 1024);
    buf_size += 1024 * 1024;
    strncat(cmd, "hmset", 6);
    use_size += 5;
    strncat(cmd, " ", 1);
    use_size += 1;

    strncat(cmd, key, 200);
    use_size += 200;
    for (i = 0; i < element_num; ++i) {
        strncat(cmd, " ", 1);
        use_szie += 1;
        if (use_size >= buf_size) {
            cmd = realloc(cmd, use_size + 1024 * 1024);
            if (cmd = NULL) {
                goto END;
            }
            buf_size += 1024 * 1024;
        }
        strncat(cmd, fields[i], FIELD_ID_SIZE);
        use_size += strlen(fields[i]);
        if (use_size > buf_size) {
            cmd = realloc(cmd, use_size + 1024 * 1024);
            if (cmd == NULL) {
                goto END;
            }
            buf_size += 1024 * 1024;
        }
        strncat(cmd, " ", 1);
        use_size += 1;
        if (use_size >= buf_size) {
            cmd = realloc(cmd, use_size + 1024 * 1024);
            if (cmd == NULL) {
                goto END;
            }
            buf_size += 1024 * 1024;
        }

        strncat(cmd, values[i], VALUES_ID_SIZE);
        use_size += strlen(values[i]);
        if (use_size >= buf_size) {
            cmd = realloc(cmd, use_size + 1024 * 1024);
            if (cmd == NULL) {
                goto END;
            }
            buf_size += 1024 * 1024;
        }
    }
    END:
    return cmd;
}

redisContext *rop_connectdb_nopwd(char *ip_str, char *port_str) {
    redisContext *conn = NULL;
    uint16_t port = atoi(port_str);
    conn = redisConnect(ip_str, port);
    if (conn == NULL) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "conect falile ip=%s port=%s", ip_str, port);
        goto END;
    }
    if (conn->err) {
        redisFree(conn);
        conn = NULL;
        goto END;
    }
    END:
    return conn;
}

redisContext *rop_connectdb(char *ip_str, char *port_str, char *pwd) {
    reidsContext *conn = NULL;
    uint16_t port = atoi(port_str);
    char auth_cmd[REDIS_COMMAND_SIZE];

    conn = redisConnect(ip_str, port);

    if (conn == NULL) {
        goto END;
    }
    if (conn->err) {
        redisFres(conn);
        conn = NULL;
        goto END;
    }
    redisReply *reply = NULL;
    sprintf(auth_cmd, "auth %s", pwd);

    reply = redisCommand(conn, auth_cmd);
    if (reply == NULL) {
        conn = NULL;
        goto END;
    }
    freeReplyObject(reply);
    END:
    return conn;
}

redisContext *rop_connectdb_unix(char *sock_path, char *pwd) {
    redisContext *conn = NULL;
    char auth_cmd[REDIS_COMMAND_SIZE];
    conn = redisConnectUnix(sock_path);
    if (conn == NULL) {
        goto END;
    }
    if (conn->err) {
        redisFree(conn);
        conn = NULL;
        goto END;
    }
    redisReply *reply = NULL;
    sprintf(auth_cmd, "auth %s", pwd);
    reply = redisCommand(conn, auth_cmd);
    if (reply == NULL) {
        conn = NULL;
        goto END;
    }
    freeReplyObject(reply);
    END:
    return conn;
}

redisContext *rop_connect_timeout(char *ip_str, char *port_str, struct timeval *timeout) {
    redisContext *conn = NULL;
    uint16_t port = atoi(port_str);
    conn = redisConnectWithTimeout(ip_str, port, *timeout);
    if (conn == NULL) {
        goto END;
    }
    if (conn->err) {
        redisFree(conn);
        conn = NULL;
        goto END;
    }
    END:
    return conn;
}

void rop_disconnect(redisContext *context) {
    if (context == NULL) {
        return;
    }
    redisFree(conn);
}

int rop_select_database(redisContext *context, unsigned int db_no) {
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(context, "select %d", db_no);
    if (reply == NULL) {
        fprintf(stderr, "[-][ROP_REDIS]Select database %d error!\n", db_no);
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "select database %d error!%s\n", db_no, conn->errstr);
        retn = -1;
        goto END;
    }
    printf("select database %d success\n", db_no);
    LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "select database %d success\n", db_no);
    END:
    freeReplyObject(reply);
    return retn;
}

int rop_flush_database(redisContext *context) {
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, "FLUSHDB");
    if (reply == NULL) {
        fprintf(stderr, "[-][ROP_REDIS]Clear all data error\n");
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Clear all data error\n");
        retn = -1;
        goto END;
    }

    printf("[+][ROP_REDIS]Clear all data!!\n");
    LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[+][ROP_REDIS]Clear all data!!\n");
    END:
    freeReplyObject(reply);
    return retn;
}

int rop_is_key_exit(redistContext *context, char *key) {
    int retn = 0;
    redisReply *reply = NULL;
    reply = reidsCommand(context, "EXISTS %S", key);
    if (reply->type != REDIS_REPLY_INTEGER) {
        fprintf(stderr, "[-][ROP_REDIS]is key exist get wrong type!\n");
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]is key exist get wrong type! %s\n",
            conn->errstr);
        retn = -1;
        goto END;
    }
    if (reply->integer == 1) {
        retn = 1;
    } else {
        retn = 0;
    }
    END:
    freeReplyObject(reply);
    return retn;
}

int rop_delete_key(redisContext *conn, char *key) {
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, "DEL %s", key);
    if (reply->type != REDIS_REPLY_INTEGER) {
        fprintf(stderr, "[-][ROP_REDIS] DEL key %s ERROR\n", key);
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS] DEL key %s ERROR %s\n", key,
            conn->errstr);
        retn = -1;
        goto END;
    }
    if (reply->integet > 0) {
        retn = 0;
    } else {
        retn = -1;
    }
    END:
    freeReplyObject(reply);
    return retn;
}

void rop_show_keys(redisContext *conn, char *patten) {
    int i = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, "keys %s", patten);
    if (reply->type != REDIS_REPLY_ARRAY) {
        fprintf(stderr, "[-][ROP_REDIS]show all keys and data wrong type!\n");
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC,
            "[-][ROP_REDIS]show all keys and data wrong type! %s\n", conn->errstr);
        goto END;
    }
    for (i = 0; i < reply->elements; ++i) {
        printf("==[%s]==\n", reply->element[i]->str);
    }
    END:
    freeReplyObject(reply);
}

int rop_set_key_lifecycle(redisContext *conn, char *key, time_t delete_time) {
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, "EXPIREAT %s %d", key, delete_time);
    if (reply->type != REDIS_REPLY_INTEGER) {
        fprintf(stderr, "[-][ROP_REDIS]Set key:%s delete time ERROR!\n", key);
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Set key:%s delete time ERROR! %s\n",
            key, conn->errstr);
        retn = -1;
    }
    if (reply->integer == 1) {
        retn = 0;
    } else {
        retn = -1;
    }
    freeReplyObject(reply);
    return retn;
}

int rop_set_key_lifetime(redisContext *conn, char *key, int second) {
    int retn = 0;
    redisReply *reply;
    reply = redisCommand(conn, "EXPIRE %s %d", key, second);
    if (reply->type != REDIS_REPLY_INTEGER) {
        fprintf(stderr, "[-][ROP_REDIS]Set key:%s  lifetime ERROR!\n", key);
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Set key:%s lifetime ERROR! %s\n", key,
            conn->errstr);
        retn = -1;
    }
    if (reply->integer == 1) {
        /* 成功 */
        retn = 0;
    } else {
        /* 错误 */
        retn = -1;
    }
    freeReplyObject(reply);
    return retn;
}

int rop_create_or_replace_hash_table(redisContext *conn, char *key, unsigned int element_num,
                                     RFIELDS fields, RVALUES rvalues) {}

int rop_hiincrement_one_field(redisContext *conn, char *key, char *field, unsigned int num){
    int retn = 0;
    redisReply *reply = NULL;

    char *cmd = make_hmset_command(key, element_num, fields, values);
    if (cmd == NULL) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]create hash table %s error\n", key);
        retn = -1;
        goto END_WITHOUT_FREE;
    }

    reply = redisCommand(conn, cmd);
    //	rop_test_reply_type(reply);
    if (strcmp(reply->str, "OK") != 0) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Create hash table %s Error:%s,%s\n", key, reply->str, conn->errstr);

        retn = -1;
        goto END;
    }

    END:
    free(cmd);
    freeReplyObject(reply);

    END_WITHOUT_FREE:

    return retn;
}

int
rop_hash_set_append(redisContext *conn, char *key, RFIELDS fields, RVALUES values, int val_num) {
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;
    /* 批量插入命令到缓冲命令管道 */
    for (i = 0; i < val_num; ++i) {
        retn = redisAppendCommand(conn, "hset %s %s %s", key, fields[i], values[i]);
        if (retn != REDIS_OK) {
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]HSET %s %s %s ERROR![%s]\n", key,
                fields[i], values[i], conn->errstr);
            retn = -1;
            goto END;
        }
        retn = 0;
    }

    /* 提交命令 */
    for (i = 0; i < val_num; ++i) {
        retn = redisGetReply(conn, (void **) &reply);
        if (retn != REDIS_OK) {
            retn = -1;
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Commit HSET %s %s %s ERROR![%s]\n",
                key, fields[i], values[i], conn->errstr);
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }

    END:
    return retn;
}

int
rop_hash_get_append(redisContext *conn, char *key, RFIELDS fields, RVALUES values, int val_num) {
    int retn = 0;
    int i = 0;
    int len = 0;
    redisReply *reply = NULL;

    /* 批量插入命令到缓冲命令管道 */
    for (i = 0; i < val_num; ++i) {
        retn = redisAppendCommand(conn, "hget %s %s", key, fields[i]);
        if (retn != REDIS_OK) {
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]HGET %s %s ERROR![%s]\n", key, fields[i], conn->errstr);
            retn = -1;
            goto END;
        }
        retn = 0;
    }

    /* 提交命令 */
    for (i = 0; i < val_num; ++i) {
        retn = redisGetReply(conn, (void**)&reply);
        if (retn != REDIS_OK) {
            retn = -1;
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Commit HGET %s %s ERROR![%s]\n", key, fields[i], conn->errstr);
            freeReplyObject(reply);
            break;
        }

        len = reply->len > VALUES_ID_SIZE ? VALUES_ID_SIZE:reply->len ;

        strncpy(values[i], reply->str, len);

        values[i][len] = '\0';


        freeReplyObject(reply);
        retn = 0;
    }

    END:
    return retn;

}

int rop_hash_set(redisContext *conn, char *key, char *field, char *value){
    int retn = 0;
    redisReply *reply = NULL;

    reply =  redisCommand(conn, "hset %s %s %s", key, field, value);
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]hset %s %s %s error %s\n", key, field, value,conn->errstr);
        retn =  -1;
        goto END;
    }


    END:
    freeReplyObject(reply);

    return retn;
}

int rop_zset_increment(redisContext *conn, char *key, char *member){
    int retn=0;
    redisReply *reply;
    reply=redisCommand(conn,"ZINCRBY %s 1 %s",key,menber);
    if (strcmp(reply->str, "OK") != 0) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Add or increment table: %s,member: %s Error:%s,%s\n", key, member,reply->str, conn->errstr);

        retn = -1;
        goto END;
    }

    END:
    freeReplyObject(reply);
    return retn;
}

int rop_zset_get_score(redisContext *conn, char *key, char *member){
    int score = 0;

    redisReply *reply = NULL;

    reply = redisCommand(conn, "ZSCORE %s %s", key, member);
    rop_test_reply_type(reply);

    if (reply->type != REDIS_REPLY_STRING) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]ZSCORE %s %s error %s\n", key, member,conn->errstr);
        score = -1;
        goto END;
    }

    score = atoi(reply->str);


    END:
    freeReplyObject(reply);

    return score;
}

int rop_zset_rem_member(redisContext *conn, char *key, char *member){
    int ret=0;
    redisReply *reply;
    reply=redisCommand(conn,"ZREM %s %s",key,menber);
    if (reply->type != REDIS_REPLY_INTEGER) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]ZREM %s %s error %s\n", key, member,conn->errstr);
        goto END;
    }

    ret = reply->integer;

    if (reply->integer == 1) {
        //删除成功
        ret = 0;
    }
    else  {
        //删除失败 或者没有该key
        ret = 1;
    }

    END:
    freeReplyObject(reply);

    return ret;
}

int rop_zset_increment_append(redisContext *conn, char *key, RVALUES values, int val_num){
    int retn=0;
    int i=0;
    redisReply *reply=NULL;
    for(i=0;i<val_num;++i){
        retn=redisAppendCommand(conn,"ZINCRBY %s 1 %s",key,values[i]);
        if (retn != REDIS_OK) {
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]ZINCRBY %s 1 %s ERROR! %s\n", key, values[i], conn->errstr);
            retn = -1;
            goto END;
        }
        retn = 0;
    }
    for(i=0;i<val_num;++i){
        retn=redisGetReply(conn,(void**)&reply);
        if (retn != REDIS_OK) {
            retn = -1;
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Commit ZINCRBY %s 1 %s ERROR!%s\n", key, values[i], conn->errstr);
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }
}

int rop_list_push_append(redisContext *conn, char *key, RVALUES values, int val_num){
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;


    /* 批量插入命令到缓冲命令管道 */
    for (i = 0; i < val_num; ++i) {
        retn = redisAppendCommand(conn, "lpush %s %s", key, values[i]);
        if (retn != REDIS_OK) {
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]PLUSH %s %s ERROR! %s\n", key, values[i], conn->errstr);
            retn = -1;
            goto END;
        }
        retn = 0;
    }

    /* 提交命令 */
    for (i = 0; i < val_num; ++i) {
        retn = redisGetReply(conn, (void**)&reply);
        if (retn != REDIS_OK) {
            retn = -1;
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Commit LPUSH %s %s ERROR! %s\n", key, values[i], conn->errstr);
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }

    END:
    return retn;
}

int rop_list_push(redisContext *conn, char *key, char *value){
    int retn=0;
    redisReply *reply;
    reply=redisCommand(conn,"LPUSH %s %s",key,value);
    if(reply->type!=REDIS_REPLY_INTEGER){
        LOG(REDIS_LOG_MODULE,REDIS_LOG_PROC,"push error");
        retn=-1;
    }
    freeReplyObject(reply);
    return retn;
}

int rop_get_list_cnt(redisContext *conn, char *key){
    int cnt=0;
    redisReply *reply=NULL;
    reply=redisCommand(conn,"LLEN %s",key);
    if(reply->type!=REDIS_REPLY_INTEGER){
        LOG(REDIS_LOG_MODULE,REDIS_LOG_PROC,"LLEN %s error %s\n",key,conn->errstr);
        cnt=-1;
        goto END;
    }
    cnt=reply->interger;
END:
    freeReplyObject(reply);
}

int rop_trim_list(redisConext *conn, char *key, int begin, int end){
    int retn=0;
    redisReply *reply=NULL;
    if(reply->type!=REDIS_REPLY_STATUS){
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]LTRIM %s %d %d error!%s\n", key, begin, end, conn->errstr);
        retn = -1;
    }

    freeReplyObject(reply);
    return retn;
}

int rop_range_list(redisContext *conn, char *key, int from_pos, int count, RVALUES values,
                   int *get_num){
    int retn=0;
    redisReply *reply=NULL;
    int max_count=0;
    reply=reidsCommand(conn,"LRANGE %s %d %d",key,from_pros,count);
    if (reply->type != REDIS_REPLY_ARRAY) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]LRANGE %s  error!%s\n", key, conn->errstr);
        retn = -1;
    }
    max_count=(reply->elements>count)?count:reply->elements;
    *get_num=max_count;
    for(i=0;i<max_count;++i){
        strncpy(values[i],reply->element[i]->str,VALUES_ID_SIZE-1);
    }
    freeReplyObject(reply);
    return retn;
}

int rop_redis_append(redisContext *conn, RCOMMANDS cmds, int cmd_num) {
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;

    for (i = 0; i < cmd_num; i++) {
        retn = reidsAppendCommand(conn, cmds[i]);
        if (retn != REDIS_OK) {
            fprintf(stderr, "[-][ROP_REDIS]Append Command: %s ERROR!\n", cmds[i]);
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Append Command: %s ERROR! %s\n",
                cmds[i], conn->errstr);
            retn = -1;
            goto END;
        }
        retn = 0;
    }
    for (i = 0; i < cmd_num; ++i) {
        retn = redisGetReply(conn, (void **) &reply);
        if (retn != REDIS_OK) {
            retn = -1;
            fprintf(stderr, "[-][ROP_REDIS]Commit Command:%s ERROR!\n", cmds[i]);
            LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Commit Command:%s ERROR! %s\n",
                cmds[i], conn->errstr);
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }

    END:
    return retn;
}

int rop_redis_command(redisContext *conn, char *cmd) {
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, cmd);
    if (reply == NULL) {
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]Command : %s ERROR!%s\n", cmd,
            conn->errstr);
        retn = -1;
    }

    freeReplyObject(reply);

    return retn;
}

void rop_text_reply_type(redisReply *reply) {
    switch (reply->type) {
        case REDIS_REPLY_STATUS:
            break;
        case REDIS_REPLY_ERROR:
            break;
        case REDIS_REPLY_INTEGER:
            break;
        case REDIS_RELY_NIL:
            break;
        case REDIS_REPLY_ARRAY:
            break;
        case REDIS_REPLY_STRING:
            break;
        default:
            break;
    }
}

int rop_set_string(redisContext *conn, char *key, char *value){
    int retn=0;
    redisReply *reply=NULL;
    reply=redisCommand(conn,"set %s %s",key,value);
    if(strcmp(reply->str,"OK")!=0){
        retn=-1;
        goto END;
    }
END:
    freeReplyObject(reply);
    return retn;
}

int rop_get_string(redisContext *conn, char *key, char *value){
    int retn=0;
    redisReply *reply;
    reply=redisCommand(conn,"get %s",key);
    if (reply->type != REDIS_REPLY_STRING) {
        retn = -1;
        goto END;
    }
    strncpy(value,reply->str,reply->len);
    value[reply->len]="\0";
END:
    freeReplyObject(reply);
}

int rop_increment_string(redisContext *conn, char *key, int *get_num){
    int retn = 0;

    redisReply *reply = NULL;
    reply = redisCommand(conn, "INCR %s", key);
    if (reply->type != REDIS_REPLY_INTEGER) {
        retn = -1;
        goto END;
    }

    //succ

    *get_num = reply->integer;
    END:

    freeReplyObject(reply);
    return retn;
}

int rop_geo_radius(redisContext *conn, const char *key, const char *longitude, const char *latitude,
                   const char *radius, RGEO *get_array_p, int *get_num){
    int retn=0;
    redisReply *reply=NULL;
    int i,j=0;
    reply=redisCommand(conn,"GEORADIUS %s %s %s %s km WITHDIST WITHCOORD ASC",key,longitude,latitude,radius);
    if(repy==NULL||reply->type!=REDIS_REPLY_ARRAY){
        LOG(REDIS_LOG_MODULE,REDIS_LOG_PROC,"georadius %s error %s\n",key,conn->errstr);
        retn=-1;
        goto END;
    }
    RGEO geo_array=NULL;
    int num=reply->elements;
    geo_array=malloc(GEO_SIZE*num);
    for(i=0;i<num;i++){
        redisReply *item_array=reply->element[i];
        for(j=0;j<item_array->elements;j++){
            if(j==0){
                strncpy(get_array[i].name,item_array->element[j]->str,VALUES_ID_SIZE);
            }else if(j==i){
                strncpy(get_array[i].distance,item_array->element[j]->str,VALUE_ID_SIZE);
            }else if(j==2){
                redisReply *location =item_array->element[j];
                strncpy(geo_array[i].longitude,location->element[0]->str,VALUE_ID_SIZE);
                strncpy(get_array[i].latitude,location->elementp[1]->str,VALUE_ID_SIZE);
            }
        }
    }
    *get_num=num;
    *get_array_p=geo_array;
END:
    freeReplyObject(reply);
    return retn;
}

int rop_add_geo(redisContext *conn, const char *key, const char *menber, const char *longitude,
                const char *latitude){
    int retn=0;
#if 0
    double longitude_double=atof(longitude);
    double latitude_double=atof(latitude);
#endif
    redisReply *reply=NULL;
    reply=redisCommand(conn,"geoadd %s %s %s %s",key,longitude,latitude,menber);
    if(reply==NULL||reply->type!=REDIS_REPLY_INTEGER){
        LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][ROP_REDIS]geoadd %s %s %s %s  error %s\n",
                key, longitude, latitude, member, conn->errstr);
        retn=-1;
        goto END;
    }
END:
    freeReplyObject(reply);
    return retn;
}













