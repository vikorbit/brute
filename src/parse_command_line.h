#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "descriptor_task.h"

namespace brute {

bool parse_alphabet(const std::string& str, std::vector<char> & alphabet);
bool parse_command_line(int argc, char **argv, brute::DescriptorTask & descriptor_task);

}