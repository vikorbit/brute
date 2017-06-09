#include "words_bucket.h"

namespace brute {

WordsBucket::WordsBucket() {};

void WordsBucket::load( size_t minLen, size_t maxLen, std::vector<char> & alphabet ) {
    if ( minLen <= 0 ) {
        throw std::invalid_argument("minLen can not be zero");
    }

    if ( maxLen <= 0 ) {
        throw std::invalid_argument("maxLen can not be zero");
    }

    if ( maxLen < minLen ) {
        throw std::invalid_argument("maxLen less than minLen");
    }

    if( alphabet.empty() ) {
        throw std::invalid_argument("alphabet is empty");
    }

    _isEmpty = false;
    _minLen = minLen;
    _maxLen = maxLen;
    _alphabet = alphabet;
    _sizeAlphabet = alphabet.size();
    _currentPos = 0;
    setStartWord( _minLen );
}

bool WordsBucket::empty() {
    return _isEmpty;
}

void WordsBucket::setStartWord( size_t len ) {
    _indexWord.resize(len);
    for ( size_t i = 0; i < len; ++i ) {
        _indexWord[ i ] = 0;
    }
}

bool WordsBucket::nextWord() {
    size_t index = (_indexWord[ _currentPos ] + 1 ) % _sizeAlphabet;
    _indexWord[ _currentPos ] = index;
    if ( index == 0 ) {
        while ( ( _currentPos + 1 ) < _indexWord.size() && index == 0 ) {
            _currentPos++;
            index = (_indexWord[ _currentPos ] + 1) % _sizeAlphabet;
            _indexWord[ _currentPos ] = index;
        }

        if ( index == 0 ) {
            size_t new_size_word = _indexWord.size() + 1;
            if ( new_size_word > _maxLen ) {
                return false;
            }

            setStartWord( new_size_word );
        }

        _currentPos = 0;
    }
    
    return true;
}

size_t WordsBucket::getVectorWords( std::vector<std::string> & vectorWords, size_t count ) {
    std::lock_guard<std::mutex> locker(_mutex);
    vectorWords.clear();

    size_t i = 0;
    while( !empty() && i < count ) {
        vectorWords.push_back( std::move( pop() ) );
        ++i;
    }

    return vectorWords.size();
}

std::string WordsBucket::pop() {
    if ( !_isEmpty ) {
        indexToString();
        std::string tmp = _currentWord;
        if ( !nextWord() ) {
            _isEmpty = true;
        }
        return tmp;
    }

    throw std::range_error("words_bucket is empty");
}

void WordsBucket::indexToString() {
    _currentWord.resize( _indexWord.size() );
    for( size_t i = 0; i < _indexWord.size(); ++i ) {
        _currentWord[i] = _alphabet[ _indexWord[ i ] ];
    }
}

}
