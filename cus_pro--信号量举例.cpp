#include <iostream>
#include <cstdio>
#include <vector>
#include <pthread.h>
#include <semaphore.h>

class RingQueue{
    private:
        std::vector<int> _list;
        int _capacity;
        int _read_step;//标记当前读取数据位置的下标
        int _write_step;//当前写入位置的下标
        sem_t _sem_lock;//这个信号量用于实现互斥
        sem_t _sem_idle;//空闲空间计数-生产者能否入队数据依据
        sem_t _sem_data;//数据资源个数-消费者能够获取数据依据
    public:
        RingQueue(int cap):_list(cap), _capacity(cap),
                _read_step(0), _write_step(0){
            //sem_init(信号量，作用范围标志，初值)
            sem_init(&_sem_lock, 0, 1);
            sem_init(&_sem_idle, 0, cap);
            sem_init(&_sem_data, 0, 0);
        }
        ~RingQueue() {
            sem_destroy(&_sem_lock);
            sem_destroy(&_sem_idle);
            sem_destroy(&_sem_data);
        }
        bool Push(const int data) {
            sem_wait(&_sem_idle);//空闲空间计数-1
            sem_wait(&_sem_lock);
            _list[_write_step] = data;
            _write_step = (_write_step+1) % _capacity;
            sem_post(&_sem_lock);
            sem_post(&_sem_data);//数据资源+1，唤醒一个消费者
            return true;
        }
        bool Pop(int *data) {
            sem_wait(&_sem_data);//资源计数-1
            sem_wait(&_sem_lock);
            *data = _list[_read_step];
            _read_step = (_read_step+1) % _capacity;
            sem_post(&_sem_lock);
            sem_post(&_sem_idle);//空闲空间+1
            return true;
        }
};

void *thr_pro(void *arg)
{
    RingQueue *queue = (RingQueue*)arg;
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
    RingQueue *queue = (RingQueue*)arg;
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
    RingQueue queue(5);
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
