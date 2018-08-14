
#include <stdio.h>
#import <pthread.h>
#import <assert.h>

static pthread_mutex_t static_mutex = PTHREAD_MUTEX_INITIALIZER;
static char asn1_locked = 0;

#define asn1_lock() {if (pthread_mutex_lock(&static_mutex)!=0){\
assert(0);\
}\
set_asn1_locked(1);\
}\

#define asn1_unlock() {if (pthread_mutex_unlock(&static_mutex)!=0){\
assert(0);\
}\
set_asn1_locked(0);\
}\

static void set_asn1_locked(char locked){
    //assert(asn1_locked!=locked);
    asn1_locked = locked;
}
