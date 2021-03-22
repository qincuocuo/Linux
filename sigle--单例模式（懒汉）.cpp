#include <iostream>
#include <mutex>

tmplate<class T>
class Singleton{
    private:
        volatile static T  *data;
        static std::mutex _mutex;
    public:
        T *getInstance() {
            if (data == NULL) {
                _mutex.lock();
                if (data == NULL)  { 
                    data = new T();  
                }
                _mutex.unlock();
            }
            return data;
        }
}
