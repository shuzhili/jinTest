//
// Created by Administrator on 2019/8/7.
//

#ifndef MY_PRO_CURL_H
#define MY_PRO_CURL_H
#include "OBOJni.h"
#include <curl/curl.h>

class Curl{

public:
    Curl(string url, bool ignoreCA);
    ~Curl();

    static size_t deal_response(void *ptr,size_t m,size_t n,void *arg);

    //向远程服务器发送请求
    bool execute(string requestData);
    //提供——responseData的getter方法
    string responseData();
private:
    CURL *_curl;
    string _responseData;//用来存储从服务器返回的数据
};
#endif //MY_PRO_CURL_H
