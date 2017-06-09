#pragma once

#include <iostream>
#include <stdlib.h>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <atomic>

#include "descriptor_task.h"
#include "words_bucket.h"


namespace brute {

class Worker;

class WorkerPool: public std::enable_shared_from_this<WorkerPool> 
{
public:
    WorkerPool( size_t count_threads );
    void run( std::shared_ptr<WordsBucket> & wordsBucket, std::function<bool(const std::string&)> predicate );
    bool isResult() const;
    std::string getResult() const;

private:
    std::string _result;
    size_t _countThreads;
    bool _isResult;
    bool _notified;
};

class Worker
{
public:
    Worker() = default;
    ~Worker();
    Worker(std::shared_ptr<WorkerPool> workerPool);
    Worker(const Worker& other);
    Worker(Worker&& other) = default;
    Worker& operator=(Worker&& other) = default;

    void run( std::shared_ptr<WordsBucket> & wordsBucket, std::function<bool(const std::string&)> predicate );
    void stop();
    bool isFinished();
    bool isResult();
    std::string getResult();

private:
    std::shared_ptr<WorkerPool> _workerPool;
    std::shared_ptr<std::thread> _thread;
    std::atomic<bool> _isWorking;
    std::string _result;
};

}
