//
// Created by Administrator on 2019/8/14.
//

#ifndef MY_PRO_MAKE_LOG_H
#define MY_PRO_MAKE_LOG_H

#include "pthread.h"

int out_out_file(char* path,char *buf);
int make_path(char *path,char *module_name,char *proc_name);
int dumpmsg_to_file(char *module_name,char *proc_name,const char *filename,
        int line,const char *funcname,char *fmt,...);

#ifndef _LOG
#define LOG(module_name,proc_name,x...)\
        do{\
            dumpmsg_to_file(module_name,proc_name,__FILE__,__LINE__,FUNCTION__,##x);\
        }while(0)
#else
#define LOG(module_name,proc_name,x...)
#endif
extern pthread_mutex_t ca_log_lock;

#endif //MY_PRO_MAKE_LOG_H















