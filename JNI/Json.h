//
// Created by Ace on 2017/8/8.
//

#ifndef MY_PRO__JSON_H
#define MY_PRO__JSON_H

#include "OBOJni.h"
#include <cJSON.h>

class Json {

public:
    Json();
    ~Json();


    //给json插入一对key value (string)
    void insert(std::string key, std::string value);

    //将json Object 转换成字符串
    std::string print();

    //将json字符串解析成一个json对象
    void parse(std::string json_str);

    //得到json中的某个key对应的value
    string value(string key);

private:
    //防止json进行拷贝， 出现浅拷贝风险
    Json(const Json &);
    Json &operator=(const Json &);

    cJSON *_root;

};


#endif //OBO_170325_JSON_H
