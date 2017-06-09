#include "worker_pool.h"

#define TIMEOUT_DURATION_MS         500
#define REQUESTED_VECTOR_SIZE       500

namespace brute {


WorkerPool::WorkerPool( size_t count_threads ): _countThreads(count_threads), _result(), _isResult(false) {}

void WorkerPool::run( std::shared_ptr<WordsBucket> & wordsBucket, std::function<bool(const std::string&)> predicate ) {
    std::vector<Worker> workerPool;
    workerPool.reserve( _countThreads );
    for( size_t i = 0; i < _countThreads; ++i ) {
        workerPool.push_back( Worker( shared_from_this() ) );
    }

    for( Worker & worker : workerPool ) {
        worker.run( wordsBucket, predicate );
    }

    _result.clear();
    _isResult = false;
    size_t count_finished_workers = 0;
    do {
        std::this_thread::sleep_for( std::chrono::milliseconds( TIMEOUT_DURATION_MS ) );
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
    } while ( count_finished_workers < workerPool.size() );
}

bool WorkerPool::isResult() const {
    return _isResult;
}

std::string WorkerPool::getResult() const {
    return _result;
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
                        _isWorking.store(false, std::memory_order_seq_cst);
                        return;
                    }
                    ++i;
                }
            }
            else {
                _isWorking.store(false, std::memory_order_seq_cst);
            }
        }
    } ) );
}

bool Worker::isFinished() {
    return !_isWorking.load( std::memory_order_seq_cst );
}

bool Worker::isResult() {
    if ( !_isWorking.load( std::memory_order_seq_cst ) ) {
        return !_result.empty();
    }
    else {
        return false;
    }
}

std::string Worker::getResult() {
    return _result;
}

}
