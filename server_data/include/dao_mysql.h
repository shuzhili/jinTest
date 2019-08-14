//
// Created by Administrator on 2019/8/13.
//
#ifndef MY_PRO_DAO_MYSQL_H
#define MY_PRO_DAO_MYSQL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mysql.h>

#define MYSQL_USER          "root"
#define MYSQL_PWD           "itcast"
#define MYSQL_DATABASE      "dstorage"
#define SQL_MAX_LEN         (1024)

void print_error(MYSQL *conn,const char *title);

void process_result_set(MYSQL *conn,MYSQL_RES *res_set);

MYSQL* mysql_conn(char *user_name,char *passwd,char *db_name);
#endif //MY_PRO_DAO_MYSQL_H
