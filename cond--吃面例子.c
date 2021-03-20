#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int bowl = 0;
pthread_cond_t cook_cond;
pthread_cond_t cus_cond;
pthread_mutex_t mutex;
void *thr_cus(void *arg)
{
    while(1) {
        pthread_mutex_lock(&mutex);
        while (bowl == 0) {
            //通过条件变量的等待接口使线程阻塞
            //pthread_cond_wait--包含了解锁进入休眠的原子操作
            //pthread_cond_wait--包含了被唤醒后加锁的操作
            pthread_cond_wait(&cus_cond, &mutex);
        }
        printf("面真好吃~~\n");
        bowl = 0;
        //唤醒厨师
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cook_cond);
    }
    return NULL;
}
void *thr_cook(void *arg)
{
    while(1) {
        pthread_mutex_lock(&mutex);
        while(bowl == 1) {
            //有饭-则等待，使用条件变量的接口进行等待
            pthread_cond_wait(&cook_cond, &mutex);
        }
        printf("做了一碗兰州拉面~~\n");
        bowl = 1;
        //唤醒顾客吃面
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cus_cond);
    }
    return NULL;
}
int main (int argc, char *argv[])
{
    int ret;
    pthread_t cus_tid, cook_tid;
    pthread_cond_init(&cus_cond, NULL);
    pthread_cond_init(&cook_cond, NULL);
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&cus_tid, NULL, thr_cus, NULL);
        if (ret != 0) {
            printf("create thread error\n");
            return -1;
        }
    }

    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&cook_tid, NULL, thr_cook, NULL);
        if (ret != 0) {
            printf("create thread error\n");
            return -1;
        }
    }

    pthread_join(cus_tid, NULL);
    pthread_join(cook_tid, NULL);
    pthread_cond_destroy(&cus_cond);
    pthread_cond_destroy(&cook_cond);
    pthread_mutex_destroy(&mutex);
    return 0;
}
