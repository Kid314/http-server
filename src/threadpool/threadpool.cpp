//
// Created by kid314 on 25-7-9.
//

#include "threadpool.h"

threadpool::threadpool(size_t thread_num):is_stop(false)
{
    all_workers.reserve(thread_num);
    for (size_t i=0;i<thread_num;++i)
    {
        all_workers.emplace_back(&threadpool::work,this);
    }
}
threadpool::~threadpool()
{
    if (!is_stop)
    {
        shutdown();
    }
}
// template<typename F,typename... Args>
// auto threadpool::enqueue(F&& f,Args&&... args) -> std::future<std::invoke_result_t<F,Args...>>
// {
//     using return_type=std::invoke_result_t<F,Args...>;
//     auto task=std::make_shared<std::packaged_task<return_type>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
//     // auto task=std::make_shared<std::packaged_task<return_type>>(
//     //     [f=std::forward<F>(f), ...args=std::forward<Args>(args)]() mutable
//     //     {
//     //         return std::invoke(std::forward<F>(f),std::forward<Args>(args)...);
//     //     }
//     // );
//     std::future<return_type> future_result=task->get_future();
//     {
//         std::lock_guard<std::mutex> q_lock(queue_lock);
//         if (is_stop)
//         {
//             throw std::runtime_error("working on a stopped threadpool");
//         }
//         all_tasks.emplace([task](){(*task)();});
//         cv.notify_one();
//     }
//     return future_result;
// }

void threadpool::work()
{
    while(true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> q_lock(queue_lock);
            cv.wait(q_lock,[this]()->bool { return !all_tasks.empty()||is_stop;});
            if (is_stop&&all_tasks.empty())
            {
                return;
            }
            task=std::move(all_tasks.front());
            all_tasks.pop();
        }
        task();
    }
}
void threadpool::shutdown()
{
    {
        std::unique_lock<std::mutex> q_lock(queue_lock);
        is_stop=true;
    }
    cv.notify_all();
    //all_workers.clear();
}
