//
// Created by Administrator on 2019/8/8.
//

#ifndef MY_PRO_HTTPS_COMMON_H
#define MY_PRO_HTTPS_COMMON_H

#define COMMON_HTTPS_PORT 8080

#define COMMON_PASSCODE "R23"

void die_most_horribly_form_openssl_error(const char *fun);

void error_exit(const char *fmt,...);

void common_setup(void);
#endif //MY_PRO_HTTPS_COMMON_H
