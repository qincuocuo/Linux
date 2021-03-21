#include "threadpool.hpp"

void handler(int data)//一个数据对应的处理接口
{
    printf("---%p- get data:%d\n", pthread_self(), data);
    sleep(1);
}

int main (int argc, char *argv[])
{
    ThreadPool pool;
    for (int i = 0; i < 10; i++) {
        ThreadTask task(i, handler);//组织一个任务节点
        pool.TaskPush(task);//抛入线程池的任务队列
    }

    while(1) {sleep(1);}
    return 0;
}
