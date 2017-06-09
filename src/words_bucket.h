#pragma once

#include <stdlib.h>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>

namespace brute {

class WordsBucket
{
public:
    WordsBucket();
    void load( size_t minLen, size_t maxLen, std::vector<char> & alphabet );
    size_t getVectorWords( std::vector<std::string> & vectorWords, size_t count );
    
private:
    std::mutex _mutex;
    bool _isEmpty = true;
    size_t _minLen = 0;
    size_t _maxLen = 0;
    size_t _sizeAlphabet;
    std::vector<char> _alphabet;
    std::string _currentWord;
    size_t _currentPos = 0;

    std::vector<int> _indexWord;

    bool nextWord();
    void indexToString();
    void setStartWord( size_t len );
    std::string pop();
    bool empty();
};

}
