#pragma once

#include <vector>
#include <string>

namespace brute {

class DescriptorTask
{
public:
    size_t minLength;
    size_t maxLength;
    std::vector<char> alphabet;
    std::string hashToCheck;
};

}
