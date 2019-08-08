//
// Created by Administrator on 2019/8/7.
//

#ifndef MY_PRO_CURL_H
#define MY_PRO_CURL_H
#include "OBOJni.h"
#include <curl/curl.h>

class Curl{

public:
    Curl(std::string url, bool ignoreCA);
    ~Curl();

    static size_t deal_response(void *ptr,size_t m,size_t n,void *arg);

    //向远程服务器发送请求
    bool execute(std::string requestData);
    //提供——responseData的getter方法
    std::string responseData();
private:
    CURL *_curl;
    std::string _responseData;//用来存储从服务器返回的数据
};
#endif //MY_PRO_CURL_H
