#include <iostream>
#include <stdlib.h>
#include <thread>

#include "md5.h"
#include "parse_command_line.h"
#include "worker_pool.h"
#include "words_bucket.h"

int main( int argc, char **argv ) {
    brute::DescriptorTask descriptor_task;
    try {
        brute::parse_command_line(argc, argv, descriptor_task);
        auto workerPool = std::make_shared<brute::WorkerPool>( std::thread::hardware_concurrency() );
        auto wordsBucket = std::make_shared<brute::WordsBucket>();
        wordsBucket->load( descriptor_task.minLength, descriptor_task.maxLength, descriptor_task.alphabet );

        std::string hashToCheck = descriptor_task.hashToCheck;
        std::function<bool(const std::string&)> predicate = [ hashToCheck ]( const std::string& str ) { return md5( str ) == hashToCheck; };

        workerPool->run( wordsBucket, predicate );
        if( workerPool->isResult() ) {
            std::cout << "Hash <" << hashToCheck << "> is <" << workerPool->getResult() << ">" << std::endl;
        }
        else {
            std::cout << "Solution not found" << std::endl;
        }
    } catch (std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Undefined exception" << std::endl;
        return 1;
    }

    return 0;
}
