#include"ThreadPool.h"

ThreadPool::ThreadPool(size_t numThread)
{
    for(size_t i=0;i<numThread;++i)
    {
        threads_.emplace_back([this]{this->worker();});
    }
}

ThreadPool::~ThreadPool()
{
    stop_.store(true);
    cond_.notify_all();
    for(auto& t:threads_)
    {
        if(t.joinable()) t.join();
    }
}

#include<syscall.h>
#include<unistd.h>

void ThreadPool::worker()
{
    printf("create thread(%ld).\n", syscall(SYS_gettid));
    while(!stop_.load())
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cond_.wait(lock, [this]{
                return stop_.load()||!tasks_.empty();
            });
            if(stop_.load()&&tasks_.empty()) return;

            task=std::move(tasks_.front());
            tasks_.pop();
        }
        try
        {
            task();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mtx_);
        tasks_.push(std::move(task));
    }
    cond_.notify_one();
}