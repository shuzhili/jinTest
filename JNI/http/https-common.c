//
// Created by Administrator on 2019/8/8.
//
#include <signal.h>
#include "https-common.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <event2/event.h>

void die_most_horribly_from_openssl_error(const char *func){
    fprintf(stderr,"%s failed:\n",func);
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}
void error_exit(const char *fmt,...){
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stderr,fmt,ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}
static void *my_zeroing_malloc(size_t howmuch){
    return calloc(1,howmuch);
}
void common_setup(void){
    signal(SIGPIPE,SIG_IGN);
    CRYPTO_set_men_functions(my_zeroing_malloc,realloc,free);
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    printf("Useing OpenSSL version \"%s\"\nand libevent version \"%s\"\n",
            SSLeay_version(SSLEAY_VERSION),event_get_version());
}