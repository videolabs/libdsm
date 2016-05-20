
#ifndef __SPNEGO_ASN1_CONF_H_
#define __SPNEGO_ASN1_CONF_H_

#include "config.h"
#include "libtasn1.h"
#import <pthread.h>

extern const asn1_static_node spnego_asn1_conf[];

static pthread_mutex_t asn1_mutex(){
    static pthread_mutex_t result = PTHREAD_MUTEX_INITIALIZER;
    return result;
}

static void asn1_lock(){
    pthread_mutex_t mutex = asn1_mutex();
    pthread_mutex_lock(&mutex);
}

static void asn1_unlock(){
    pthread_mutex_t mutex = asn1_mutex();
    pthread_mutex_unlock(&mutex);
}

#endif
