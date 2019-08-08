//
// Created by Ace on 2017/8/8.
//

#include "Json.h"


Json::Json()
{
    this->_root = cJSON_CreateObject();
}

Json::~Json()
{
    cJSON_Delete(this->_root);
}


//给json插入一对key value (string)
void Json::insert(string key, string value)
{
    cJSON_AddStringToObject(this->_root, key.c_str(), value.c_str());
}


//将json Object 转换成字符串
string Json::print()
{
    char *json_str = cJSON_Print(this->_root);

    string str = string(json_str);

    return str;
}

//将json字符串解析成一个json对象
void Json::parse(string json_str)
{
    cJSON *root = cJSON_CreateObject();


    root = cJSON_Parse(json_str.c_str());

    //将之前的_root释放
    cJSON_Delete(this->_root);

    this->_root = root;
}

//得到json中的某个key对应的value
string Json::value(string key)
{
    cJSON* obj = cJSON_GetObjectItem(this->_root, key.c_str());

    if (obj == NULL) {
        return string();
    }


    return string(obj->valuestring);
}