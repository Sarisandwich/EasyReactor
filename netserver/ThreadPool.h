#pragma once

#include<iostream>
#include<vector>
#include<thread>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<atomic>

class ThreadPool
{
private:
    std::vector<std::thread> threads_;  //线程池中的线程集合。
    std::queue<std::function<void()>> tasks_;    //任务队列。
    std::mutex mtx_;    //互斥锁。
    std::condition_variable cond_;      //任务队列同步的条件变量。
    std::atomic<bool> stop_{false};     //停止标志。

    std::string threadType_;    //线程类型。如"IO","WORKS".

    void worker();  //每个线程的主函数。
public:
    explicit ThreadPool(size_t numThreads=std::thread::hardware_concurrency(), const std::string& threadType="WORKS");
    ~ThreadPool();

    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;

    void enqueue(std::function<void()> task);
};