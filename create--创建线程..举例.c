#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

void *thread_entry(void *arg)
{
    //pthread_self() 获取线程自身ID
    pthread_detach(pthread_self());//自己分离自己
    char *ptr = "这是线程的返回值";
    while(1) {
        printf("this is child thread-%s\n", (char*)arg);
        sleep(1);
        //pthread_exit((void*)ptr);
    }
    return NULL;
}
int main (int argc, char *argv[])
{
    char *arg = "明天可以休息，好爽~~\n";
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thread_entry, arg);
    if (ret != 0) {
        printf("thread create error\n");
        return -1;
    }

    //pthread_detach(tid);
    ret = pthread_join(tid, NULL);
    if (ret == EINVAL) {
        printf("thread is not a joinable thread\n");
    }
    //pthread_cancel(tid);
    //char *retval;
    //pthread_join(tid, (void**)&retval);
    //printf("%d\n", (int)retval);
    //pthread_exit(NULL);
    printf("%p\n", tid);
    while(1) {
        printf("this is main thread!!\n");
        sleep(1);
    }
    return 0;
}
