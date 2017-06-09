#include "worker_pool.h"

#define TIMEOUT_DURATION_MS         100
#define REQUESTED_VECTOR_SIZE       500

namespace brute {

WorkerPool::WorkerPool( size_t count_threads ): _countThreads(count_threads), _result(), _isResult(false) {}

void WorkerPool::run( std::shared_ptr<WordsBucket> & wordsBucket, std::function<bool(const std::string&)> predicate ) {
    std::unique_lock<std::timed_mutex> lock_mutex(_mutex);

    std::vector<Worker> workerPool;
    workerPool.reserve( _countThreads );
    for( size_t i = 0; i < _countThreads; ++i ) {
        workerPool.push_back( Worker( shared_from_this() ) );
    }

    for( Worker & worker : workerPool ) {
        worker.run( wordsBucket, predicate );
    }

    _result.clear();
    size_t count_finished_workers = 0;
    do {
        _conditionVar.wait(lock_mutex);
        count_finished_workers = 0;
        for ( Worker & worker : workerPool) {
            if ( worker.isFinished() ) {
                count_finished_workers++;
                if ( worker.isResult() ) {
                    _result = worker.getResult();
                    _isResult = true;
                    return;
                }
            }
        }
    } while ( !_isResult && count_finished_workers < workerPool.size() );
}

bool WorkerPool::isResult() const {
    return _isResult;
}

std::string WorkerPool::getResult() const {
    return _result;
}

bool WorkerPool::signalFinished()
{
    if ( _mutex.try_lock_for( std::chrono::milliseconds( TIMEOUT_DURATION_MS ) ) ) {
        _conditionVar.notify_one();
        _mutex.unlock();
        return true;
    } else {
        return false;
    }
}


Worker::Worker(std::shared_ptr<WorkerPool> workerPool): _workerPool(workerPool), _thread(nullptr), _isWorking(false), _result()  {}

Worker::~Worker() {
    stop();
}

Worker::Worker(const Worker& other): 
    _workerPool(other._workerPool), 
    _thread(other._thread),
    _result(other._result),
    _isWorking(other._isWorking.load() ) 
{
}

void Worker::stop() {
    _isWorking.store(false, std::memory_order_seq_cst);
    if ( _thread ) {
        if ( _thread->joinable() ) {
            _thread->join();
        }
        _thread.reset();
    }
}

void Worker::run( std::shared_ptr<WordsBucket> & wordsBucket, std::function<bool(const std::string&)> predicate ) {
    stop();
    _isFinished.store(false, std::memory_order_seq_cst);
    _result.clear();
    _thread.reset( new std::thread( [this, predicate, &wordsBucket ] () {
        _isWorking.store(true, std::memory_order_seq_cst);
        std::vector<std::string> vectorWords;
        while( _isWorking.load( std::memory_order_seq_cst ) ) {
            wordsBucket->getVectorWords( vectorWords, REQUESTED_VECTOR_SIZE );
            if ( vectorWords.size() != 0 ) {
                size_t i = 0;
                while ( _isWorking.load( std::memory_order_seq_cst ) && i < vectorWords.size() ) {
                    if ( predicate( vectorWords[i] ) ) {
                        _result = vectorWords[i];
                        signalFinished();
                        return;
                    }
                    ++i;
                }
            }
            else {
                break;
            }
        }
        signalFinished();
    } ) );
}

void Worker::signalFinished() {
    std::lock_guard<std::mutex> locker(_mutex);
    _isFinished.store(true, std::memory_order_seq_cst);
    while ( _isWorking.load( std::memory_order_seq_cst ) ) {
        if ( _workerPool->signalFinished() ) {
            break;
        }
        else {
            std::this_thread::sleep_for( std::chrono::milliseconds( TIMEOUT_DURATION_MS ) );
        }
    }

    _isWorking.store(false, std::memory_order_seq_cst);
}

bool Worker::isFinished() {
    std::lock_guard<std::mutex> locker(_mutex);
    return _isFinished.load( std::memory_order_seq_cst );
}

bool Worker::isResult() {
    std::lock_guard<std::mutex> locker(_mutex);
    if ( !_isWorking.load( std::memory_order_seq_cst ) ) {
        return !_result.empty();
    }
    else {
        return false;
    }
}

std::string Worker::getResult() {
    std::lock_guard<std::mutex> locker(_mutex);
    return _result;
}

}
