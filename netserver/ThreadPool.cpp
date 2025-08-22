#include"ThreadPool.h"

ThreadPool::ThreadPool(size_t numThread, const std::string& threadType): threadType_(threadType)
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
    printf("create %s thread(%ld).\n", threadType_.c_str(), syscall(SYS_gettid));
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
            printf("%s thread(%ld) execute task.\n", threadType_.c_str(), syscall(SYS_gettid));
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

size_t ThreadPool::size()
{
    return threads_.size();
}