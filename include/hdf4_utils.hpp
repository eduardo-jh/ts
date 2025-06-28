#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <map>
#include <fstream>
#include <string>
// #include <filesystem>
#include <sstream>

#include "mfhdf.h"  // HDF4 Scientific Dataset API
#include "stats_utils.hpp"

std::string get_hdf4_type_name(int32_t data_type);
std::vector<std::string> read_hdf4_info(const std::string& file_path);
std::vector<int16_t> read_mask_dataset_hdf4(const std::string& mask_path);
void read_hdf4_file_create_csv(const std::string& file_path,
                          std::vector<int16_t>& mask,
                          std::vector<int16_t>& mask_values,
                          const float& vmin,
                          const float& vmax,
                          const std::string& csv_file,
                          const std::string& date_str,
                          const std::map<std::string, std::pair<float, float>>& dataset_ranges);