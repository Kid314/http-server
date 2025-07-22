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
    auto enqueue(F&& f,Args&&... args) -> std::future<std::invoke_result_t<F,Args...>>
    {
        using return_type=std::invoke_result_t<F,Args...>;
        auto task=std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
        // auto task=std::make_shared<std::packaged_task<return_type>>(
        //     [f=std::forward<F>(f), ...args=std::forward<Args>(args)]() mutable
        //     {
        //         return std::invoke(std::forward<F>(f),std::forward<Args>(args)...);
        //     }
        // );
        std::future<return_type> future_result=task->get_future();
        {
            std::lock_guard<std::mutex> q_lock(queue_lock);
            if (is_stop)
            {
                throw std::runtime_error("working on a stopped threadpool");
            }
            all_tasks.emplace([task](){(*task)();});
            cv.notify_one();
        }
        return future_result;
    }
};



#endif //THREADPOOL_H
