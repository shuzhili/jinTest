//
// Created by Administrator on 2019/8/15.
//

#ifndef MY_PRO_BUSI_CB_H
#define MY_PRO_BUSI_CB_H

#define MYHTTPD_SIGNATURE       "MoCarHttpd v0.1"

void persistent_store_cb(struct evhttp_request *req,void *arg);
void cache_store_cb(struct evhttp_request *req,void *arg);

char* deal_persistent(char *request_data_buf);
char* deal_cache(char *request_data_buf);

#endif //MY_PRO_BUSI_CB_H
