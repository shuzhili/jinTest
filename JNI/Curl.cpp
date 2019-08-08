//
// Created by Administrator on 2019/8/7.
//
#include "Curl.h"
#include "curl/curl.h"

Curl::Curl(std::string url, bool ignoreCA) {
    this->_curl=curl_easy_init();
    curl_easy_setopt(this->_curl,CURLOPT_URL,url.c_str());

    if(ignoreCA==true){
        curl_easy_setopt(this->_curl,CURLOPT_SSL_VERIFYHOST, false);
        curl_easy_setopt(this->_curl,CURLOPT_SSL_VERIFYPEER,false);
    }
}
Curl::~Curl(){
    curl_easy_cleanup(this->_curl);
}
std::string Curl::responseData() {
    return this->_responseData;
}

size_t Curl::deal_response(void *ptr, size_t m, size_t n, void *arg) {
    Curl *This=(Curl*)arg;
    char *p=(char*)ptr;
    int count=m*n;
    std::copy(p,p+count,std::back_inserter(This->_responseData));
}
bool Curl::execute(std::string requestData) {
    CURLcode res;
    //设置post请求
    curl_easy_setopt(this->_curl,CURLOPT_POST,true);
    //设置传递的post数据
    curl_easy_setopt(this->_curl,CURLOPT_POSTFIELDS,requestData.c_str());
    //设置curl处理服务器数据回调函数
    curl_easy_setopt(this->_curl,CURLOPT_WRITEFUNCTION,deal_response);

    curl_easy_setopt(this->_curl,CURLOPT_WRITEDATA,this);

    res=curl_easy_perform(this->_curl);
    if(res!=CURLE_OK){
        __android_log_print(ANDROID_LOG_ERROR,TAG,"curl:perform ERROR, rescode= [%d]\n", res);
        return false;
    }
    return true;
}



