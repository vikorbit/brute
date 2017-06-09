#include "parse_command_line.h"

namespace brute {

bool parse_alphabet(const std::string& str, std::vector<char> & alphabet) {
    alphabet.clear();
    std::stringstream ss;
    ss << str;
    try {
        while (!ss.eof()) {
            char ch;
            unsigned int beg, end;
            ss >> beg;
            ss >> ch;
            if ( ch != '-' )
                return false;

            ss >> end;

            for ( unsigned int i = beg; i <= end; ++i ) {
                alphabet.push_back(i);
            }

            if (!ss.eof()) {
                ss >> ch;
                if (ch != ':')
                return false;
            }
        }
    } catch (...) {
        return false;
    }

    if ( alphabet.empty() ) {
        return false;
    }
    return true;
}

bool parse_command_line(int argc, char **argv, brute::DescriptorTask & descriptor_task) {
    if ( argc != 5 ) {
        std::cout << "Usage: " << argv[0] << " <minLength> <maxLength> <alphabet> <hashToCheck>" << std::endl; // ./brute 3 14 65-90:97-122 hashToCheck
        return false;
    }

    descriptor_task.minLength = atol( argv[1] );
    descriptor_task.maxLength = atol( argv[2] );
    descriptor_task.hashToCheck = std::string( argv[4] );
    return parse_alphabet(argv[3], descriptor_task.alphabet);
}

}
