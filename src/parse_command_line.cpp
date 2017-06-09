#include "parse_command_line.h"

namespace brute {

void parse_alphabet(const std::string& str, std::vector<char> & alphabet) {
    alphabet.clear();
    std::stringstream ss;
    ss << str;
    try {
        while (!ss.eof()) {
            char ch;
            unsigned int beg, end;
            ss >> beg;
            ss >> ch;
            if ( ch != '-' ) {
                throw std::invalid_argument("not valid format keyspace: " + str);
            }

            ss >> end;

            for ( unsigned int i = beg; i <= end; ++i ) {
                alphabet.push_back(i);
            }

            if (!ss.eof()) {
                ss >> ch;
                if (ch != ':') {
                    throw std::invalid_argument("not valid format keyspace: " + str);
                }
                
            }
        }
    } catch (...) {
        throw;
    }

    if ( alphabet.empty() ) {
        throw std::invalid_argument("alphabet is empty");
    }
}

void parse_command_line(int argc, char **argv, brute::DescriptorTask & descriptor_task) {
    if ( argc != 5 ) {
        throw std::invalid_argument("Usage: " + std::string( argv[0] ) + " <minLength> <maxLength> <alphabet> <hashToCheck>"); // ./brute 3 14 65-90:97-122 hashToCheck
    }

    descriptor_task.minLength = atol( argv[1] );
    descriptor_task.maxLength = atol( argv[2] );
    descriptor_task.hashToCheck = std::string( argv[4] );

    if ( descriptor_task.hashToCheck.size() != 32 ) {
        throw std::invalid_argument("hashToCheck encoded in hex must have a length of 32 characters ");
    }

    parse_alphabet(argv[3], descriptor_task.alphabet);
}

}
