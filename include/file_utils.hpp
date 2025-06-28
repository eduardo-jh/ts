#pragma once

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <filesystem>

#include "hdf4_utils.hpp"
#include "hdf5_utils.hpp"

// namespace fs = std::filesystem;

// fs::path find_first_valid_file(const std::string& root,
std::filesystem::path find_first_valid_file(const std::string& root,
                               const std::string& extension,
                               const std::vector<std::string>& subdirs);

std::string convert_doy_date(const std::string& filename);

void scan_directory_tree(const std::string& root,
                         const std::string& format,
                         const std::vector<std::string>& subdirs,
                         const std::string& mask_path,
                         std::vector<int16_t> mask_values,
                         const float& vmin,
                         const float& vmax,
                         const std::string& csv_file,
                         const std::map<std::string, std::pair<float, float>>& dataset_ranges);
