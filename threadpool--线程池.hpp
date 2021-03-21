#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <pthread.h>

typedef void (*handler_t)(int data);

class ThreadTask
{
    public:
        ThreadTask() {}
        ThreadTask(int data, handler_t hander):
            _data(data), _handler(hander){}

        void SetTask(int data, handler_t hander) {
            _data = data;
            _handler = hander;
            return ;
        }
        void Run() {
            return _handler(_data);
        }
    private:
        int _data;
        handler_t _handler;
};
#define THR_MAX 5
#define QUE_MAX 10
class ThreadPool
{
    public:
        ThreadPool(int thr_max=THR_MAX, int que_max=QUE_MAX){
            pthread_mutex_init(&_mutex, NULL);
            pthread_cond_init(&_cond_pro, NULL);
            pthread_cond_init(&_cond_cus, NULL);
            int ret;
            pthread_t tid;
            for (int i = 0; i < thr_max; i++) {
                //void *(*thread_routine)(void *);
                ret = pthread_create(&tid, NULL, 
                        thr_start, this);
                if (ret != 0) {
                    printf("create thread error\n");
                    exit(-1);
                }
                pthread_detach(tid);
            }
        }
        ~ThreadPool(){
            pthread_mutex_destroy(&_mutex);
            pthread_cond_destroy(&_cond_pro);
            pthread_cond_destroy(&_cond_cus);
        }
        bool TaskPush(const ThreadTask &task) {
            //线程安全的任务入队
            pthread_mutex_lock(&_mutex);
            while(_queue.size() == _capacity) {
                pthread_cond_wait(&_cond_pro, &_mutex);
            }
            _queue.push(task);
            pthread_mutex_unlock(&_mutex);
            pthread_cond_broadcast(&_cond_cus);
            return true;
        }
        static void *thr_start(void *arg) { 
            //pool接收了传入的this对象指针
            ThreadPool *pool = (ThreadPool*)arg;
            while(1) {
                pthread_mutex_lock(&pool->_mutex);
                while(pool->_queue.empty()) {
                    pthread_cond_wait(&pool->_cond_cus, 
                            &pool->_mutex);
                }
                ThreadTask task;
                task = pool->_queue.front();
                pool->_queue.pop();
                pthread_mutex_unlock(&pool->_mutex);
                //mutex只保护queue
                pthread_cond_broadcast(&pool->_cond_pro);
                task.Run();
            }
            return NULL; 
        }
    private:
        int _thread_max;
        int _capacity;
        std::queue<ThreadTask> _queue;
        pthread_mutex_t _mutex;
        pthread_cond_t _cond_pro;
        pthread_cond_t _cond_cus;
};
