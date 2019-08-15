//
// Created by Administrator on 2019/8/15.
//

#include <stdio.h>
#include <stdlib.h>
#include "include/util.h"
#include "include/cJSON.h"

db_config_t g_db_config;

void config_init(){
    GetProfileString(CONFIG_PATH,"redis","ip",g_db_config.cache_ip);
    GetProfileString(CONFIG_PATH,"redis","port",g_db_config.cache_port);

    GetProfileString(CONFIG_PATH,"mysql","ip",g_db_config.db_ip);
    GetProfileString(CONFIG_PATH, "mysql", "port", g_db_config.db_port);
    GetProfileString(CONFIG_PATH, "mysql", "database", g_db_config.db_basename);
    GetProfileString(CONFIG_PATH, "mysql", "user", g_db_config.db_username);
    GetProfileString(CONFIG_PATH, "mysql", "passwd", g_db_config.db_passwd);
}

char *make_response_json(int ret,char *reason){
    cJSON root=cJSON_CreateObject();
    if(ret==0){
        cJSON_AddStringToObject(root,"result","ok");
    }else{
        cJSON_AddStringToObject(root, "result", "error");
        cJSON_AddStringToObject(root, "reason", reason);
    }
    char *response_data = cJSON_Print(root);
    cJSON_Delete(root);

    return response_data;
}
char *make_response_gethash_json(int ret, char *reason, RVALUES rvalues, int value_num)
{
    cJSON *root = cJSON_CreateObject();
    if (ret == 0) {
        int i = 0;
        cJSON_AddStringToObject(root, "result", "ok");
        cJSON_AddNumberToObject(root, "count", value_num);

        cJSON *values = cJSON_CreateArray();
        for (i = 0; i < value_num; i++) {
            cJSON_AddItemToArray(values, cJSON_CreateString(rvalues[i]));
        }
        cJSON_AddItemToObject(root, "values", values);
    }
    else {
        cJSON_AddStringToObject(root, "result", "error");
        cJSON_AddStringToObject(root, "reason", reason);
    }


    char *response_data = cJSON_Print(root);
    cJSON_Delete(root);

    return response_data;
}
char *make_response_geo_drivers_json(int ret, char *reason, RGEO geo_array, int geo_num)
{
    cJSON *root = cJSON_CreateObject();

    if (ret == 0) {
        int i = 0;
        cJSON_AddStringToObject(root, "result", "ok");
        cJSON_AddNumberToObject(root, "count", geo_num);

        cJSON *drivers = cJSON_CreateArray();
        for (i = 0; i < geo_num; i++) {
            cJSON *driver_geo_obj = cJSON_CreateObject();

            cJSON_AddStringToObject(driver_geo_obj, "sessionid", geo_array[i].name);
            cJSON_AddStringToObject(driver_geo_obj, "distance", geo_array[i].distance);
            cJSON_AddStringToObject(driver_geo_obj, "longitude", geo_array[i].longitude);
            cJSON_AddStringToObject(driver_geo_obj, "latitude", geo_array[i].latitude);

            cJSON_AddItemToArray(drivers, driver_geo_obj);
        }
        cJSON_AddItemToObject(root, "drivers", drivers);
    }
    else {
        cJSON_AddStringToObject(root, "result", "error");
        cJSON_AddStringToObject(root, "reason", reason);
    }


    char *response_data = cJSON_Print(root);
    cJSON_Delete(root);

    return response_data;
}