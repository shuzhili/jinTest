//
// Created by Administrator on 2019/8/13.
//

#ifndef MY_PRO_CONFIG_H
#define MY_PRO_CONFIG_H
char *l_trim(char *szOutput,const char *szInput);
char *r_trim(char *szOutput,const char *szInput);
char *a_trim(char *szOutput,const char *szInput);
int GetProfileString(char *profile,char *AppName,char *keyName,char *keyVal);
#endif //MY_PRO_CONFIG_H
