//
// Created by kid314 on 25-7-9.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<vector>
#include<queue>
#include<thread>
#include<functional>
#include<condition_variable>
#include<mutex>
#include<future>
class threadpool
{
private:
    std::vector<std::jthread> all_workers;
    std::queue<std::function<void()>> all_tasks;
    std::mutex queue_lock;
    std::condition_variable cv;
    bool is_stop;
    void work();
public:
    explicit threadpool(size_t);
    ~threadpool();
    threadpool(const threadpool&)=delete;
    threadpool(threadpool&&)=delete;
    threadpool& operator=(const threadpool&)=delete;
    threadpool& operator=(threadpool&&)=delete;
    template<typename F,typename... Args>
    auto enqueue(F&& f,Args&&... args) -> std::future<std::invoke_result_t<F,Args...>>;
};



#endif //THREADPOOL_H
