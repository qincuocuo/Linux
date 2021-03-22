#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <pthread.h>

class BlockQueue
{
    public:
        BlockQueue(int cap):_capacity(cap){
            pthread_mutex_init(&_mutex, NULL);
            pthread_cond_init(&_cond_cus, NULL);
            pthread_cond_init(&_cond_pro, NULL);
        }
        ~BlockQueue(){
            pthread_mutex_destroy(&_mutex);
            pthread_cond_destroy(&_cond_cus);
            pthread_cond_destroy(&_cond_pro);
        }
        bool Push(const int data){
            pthread_mutex_lock(&_mutex);
            while(_queue.size() == _capacity) {
                pthread_cond_wait(&_cond_pro, &_mutex);
            }
            _queue.push(data);
            pthread_mutex_unlock(&_mutex);
            pthread_cond_broadcast(&_cond_cus);
            return true;
        }
        bool Pop(int *buf) {
            pthread_mutex_lock(&_mutex);
            while (_queue.empty() == true) {
                pthread_cond_wait(&_cond_cus, &_mutex);
            }
            *buf = _queue.front();
            _queue.pop();
            pthread_mutex_unlock(&_mutex);
            pthread_cond_broadcast(&_cond_pro);
            return true;
        }
    private:
        std::queue<int> _queue;
        int _capacity;
        pthread_mutex_t _mutex;
        pthread_cond_t _cond_cus;
        pthread_cond_t _cond_pro;
};

void *thr_pro(void *arg)
{
    BlockQueue *queue = (BlockQueue*)arg;
    int i = 0;//局部变量，四个生产者各有各的
    while(1) {
        //生产者不断的入队数据
        queue->Push(i);
        printf("%p-thread-put data:%d\n", pthread_self(),i++);
    }
    return NULL;
}
void *thr_cus(void *arg)
{
    BlockQueue *queue = (BlockQueue*)arg;
    while(1) {
        //消费者不断处理数据
        int d;
        queue->Pop(&d);
        printf("%p-thread-get data:%d\n", pthread_self(), d);
    }
    return NULL;
}
int main (int argc, char *argv[])
{
    int ret;
    pthread_t ptid[4], ctid[4];
    BlockQueue queue(5);
    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&ptid[i], NULL, 
                thr_pro, (void*)&queue);
        if (ret != 0) {
            printf("create thread error");
            return -1;
        }
        ret = pthread_create(&ctid[i], NULL, 
                thr_cus, (void*)&queue);
        if (ret != 0) {
            printf("create thread error");
            return -1;
        }
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(ptid[i], NULL);
        pthread_join(ctid[i], NULL);
    }
    return 0;
}
