
#import <pthread.h>

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

