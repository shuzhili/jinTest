//
// Created by Administrator on 2019/8/13.
//
#include "dao_mysql.h"

void print_error(MYSQL *conn,const char *title){
    fprintf(stderr,"%s:\nError %u (%s)\n",title,mysql_errno(conn),mysql_error(conn));
}
void process_result_set(MYSQL *conn,MYSQL_RES *res_set){
    MYSQL_ROW row;
    uint i;
    while((row=mysql_fetch_row(res_set))!=NULL){
        for(i=0;i<mysql_num_fields(res_set);i++){
            if(i>0){
                fputc('\t',stdout);
            }
            printf("%s",row[i]!=NULL?row[i]:"NULL");
        }
        fputc('\n',stdout);
    }
    if(mysql_errno(conn)!=0){
        print_error(conn,,"mysql_fetch_row() failed");
    }else{
        printf("%lu rows returned \n",(ulong)mysql_num_rows(res_set));
    }
}

MYSQL* mysql_conn(char *user_name,char * passwd,char  *db_name){
    MYSQL *conn=NULL;
    char *opt_host_name=NULL;
    uint opt_port_num=0;
    char *opt_socket_name=NULL;
    uint opt_flags=0;

    conn=mysql_init(NULL);
    if(conn==NULL){
        fprintf(stderr,"mysql init faile");
        return NULL;
    }
    if(mysql_real_connect(conn,opt_host_name,user_name,passwd,db_name,opt_port_num,opt_socket_name,opt_flags)==NULL){
        fprintf(stderr,"mysql_conn faile %u(%s)",mysql_errno(conn),mysql_err(conn));
        mysql_close(conn);
        return NULL;
    }
    return conn;
}

#if 0
int main(void){
    char buffer[SQL_MAX_LEN]={0};
    MYSQL *conn=msql_conn(MYSQL_USER,MYSQL_PWD,MYSQL_DATABASE);
    if(conn==NULL){
        return 1;
    }
    sprintf(buffer,"insert into %s (u_name,nicheng,password,phone,createtime,email) value ('%s','%s','%s','%s','%s','%s')",
            "user","itcast","heima","itcast",12345678,"2016=08-04","liudanbing@itcast.cn");
    if(mysql_query(conn,buffer)){
        print_error(conn,"插入失败");
    }else{
        printf("插入成功，受影响函数：%lu\n",(ulong)mysql_affected_rows(conn));
    }

    sprintf(buffer,"select * from %s","user");
    if(mysql_query(conn,buffer)){
        printf_error(comm,"mysql_query() error");
    }else{
        MYSQL_RES *res_set;
        res_set=mysql_store_result(conn);
        if(res_set==NULL){
            print_error(conn,"msql_store_result faile");
        }else{
            process_reslt_set(conn,res_set);
            mysql_free_result(res_set);
        }
    }
    mysql_close(conn);
    return 0;
}
#endif

















