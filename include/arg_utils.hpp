#pragma once

#include <cstdint>
#include <string>
#include <vector>

void print_help();
void print_version();
std::vector<std::string> split_list(const std::string& str);
std::vector<int16_t> split_list_int(const std::string& str);
