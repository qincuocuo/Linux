#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int tickets = 100;
pthread_mutex_t mutex;

void *thr_cattle(void *arg)
{
    while(1) {
        pthread_mutex_lock(&mutex);
        if (tickets > 0) {
            usleep(1000);
            printf("我抢到了一张票，哈哈哈~~：%d-%p\n", tickets, pthread_self());
            tickets--;
            pthread_mutex_unlock(&mutex);
        }else {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);//没票了则退出
        }
    }
}
int main (int argc, char *argv[])
{
    int num = 4, ret;
    pthread_t tid[4];
    pthread_mutex_init(&mutex, NULL);//初始化-在线程创建之前
    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&tid[i], NULL, thr_cattle, NULL);
        if (ret != 0) {
            printf("thread create error\n");
            return -1;
        }
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}
