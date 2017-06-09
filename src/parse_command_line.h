#pragma once

#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>

#include "descriptor_task.h"

namespace brute {

void parse_alphabet(const std::string& str, std::vector<char> & alphabet);
void parse_command_line(int argc, char **argv, brute::DescriptorTask & descriptor_task);

}