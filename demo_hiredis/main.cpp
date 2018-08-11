//
//  main.cpp
//  demo_hiredis
//
//  Created by lwstar on 2018/8/11.
//  Copyright © 2018年 lwstar. All rights reserved.
//

#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "redis_server.hpp"

typedef struct __thread_data {
    void* user_data;
} thread_data;

RedisServer redisSrv;

pthread_mutex_t mutext_t;
pthread_cond_t cond_t;

void* entry(void* args) {
    printf("begin\n");
    
    long long c = redisSrv.stringCommand()->setnx("config:autoincr", "1000000");
    printf("%lld\n", c);
    int i = 1000;
    while (i <= 9999) {
        {
            long long uid = redisSrv.stringCommand()->incr("config:autoincr");
            char uid_buf[64] = {0};
            sprintf(uid_buf, "%lld", uid);
            
            char phone_buf[12] = {0};
            sprintf(phone_buf, "1363276%d", i);
            
            c = redisSrv.hashCommand()->hset(phone_buf, "phone", phone_buf, "users:");
            c = redisSrv.hashCommand()->hset(phone_buf, "name", "liwei", "users:");
            c = redisSrv.hashCommand()->hset(phone_buf, "sex", "1", "users:");
            c = redisSrv.hashCommand()->hset(phone_buf, "age", "30", "users:");
            c = redisSrv.hashCommand()->hset(phone_buf, "address", "shenzhenshinanshanqu", "users:");
            c = redisSrv.hashCommand()->hset(phone_buf, "uid", uid_buf, "users:");
            
            redisSrv.hashCommand()->hset(uid_buf, "phone", phone_buf, "uids:");
        }
        
        i++;
    }

//
//    i = 1000;
//    while (i <= 9999) {
//        redisSrv.hashCommand()->hget("", "", "uids:");
//    }
    
    pthread_cond_signal(&cond_t);
    
    printf("over\n");
    
    return 0;
}

int main(int argc, const char * argv[]) {
    redisSrv.start("127.0.0.1", 6379, 1);
    pthread_mutex_init(&mutext_t, NULL);
    pthread_cond_init(&cond_t, NULL);
    
    {
        pthread_t tid;
        pthread_create(&tid, NULL, entry, NULL);
    }
    
//    {
//        pthread_t tid;
//        pthread_create(&tid, NULL, entry, NULL);
//        
//    }
    
    {
        pthread_mutex_lock(&mutext_t);
        pthread_cond_wait(&cond_t, &mutext_t);
    }

    pthread_mutex_destroy(&mutext_t);
    pthread_cond_destroy(&cond_t);
    
    return 0;
}
