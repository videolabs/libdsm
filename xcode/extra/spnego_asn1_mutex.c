//
//  spnego_asn1_mutex.c
//  dsm
//
//  Created by Artem Meleshko on 10/8/16.
//  Copyright © 2016 myorg. All rights reserved.
//


/*
#include <stdio.h>
#import <pthread.h>
#import <assert.h>

static pthread_mutex_t static_mutex = PTHREAD_MUTEX_INITIALIZER;
static char asn_locked = 0;

void asn1_lock(){
    if (pthread_mutex_lock(&static_mutex)!=0){
        assert(0);
    }
    assert(asn_locked==0);
    asn_locked = 1;
}

void asn1_unlock(){
    if (pthread_mutex_unlock(&static_mutex)!=0){
        assert(0);
    }
    assert(asn_locked==1);
    asn_locked = 0;
}
*/
