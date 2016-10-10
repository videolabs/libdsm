//
//  spnego_asn1_mutex.c
//  dsm
//
//  Created by Artem Meleshko on 10/8/16.
//  Copyright © 2016 myorg. All rights reserved.
//

#include <stdio.h>
#import <pthread.h>
#import <assert.h>

pthread_mutex_t static_mutex = PTHREAD_MUTEX_INITIALIZER;
static char asn_locked = 0;

void asn1_lock(){
    assert(asn_locked==0);
    pthread_mutex_t mutex = static_mutex;
    pthread_mutex_lock(&mutex);
    asn_locked = 1;
}

void asn1_unlock(){
    assert(asn_locked==1);
    pthread_mutex_t mutex = static_mutex;
    pthread_mutex_unlock(&mutex);
    asn_locked = 0;
}

