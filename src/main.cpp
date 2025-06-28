#include <cstdint>
#include <iostream>
#include <getopt.h>
// #include <cstdlib>
// #include <string>
// #include <filesystem>
#include <sstream>
#include <vector>
// #include <regex>
// #include <iomanip>
// #include <chrono>
// #include <limits>
// #include <cmath>
// #include <type_traits>
// #include <fstream>
#include <map>

// #include "mfhdf.h"  // HDF4 Scientific Dataset API
// #include "hdf5.h"   // HDF5

#include "arg_utils.hpp"
// #include "hdf4_utils.hpp"
// #include "hdf5_utils.hpp"
#include "file_utils.hpp"
// #include "stats_utils.hpp"

// namespace fs = std::filesystem;

// void print_help() {
//     std::cout << "Usage: ts [OPTIONS]\n"
//               << "Options:\n"
//               << "  -d, --dir PATH              Specify directory to proceess files\n"
//               << "  -f, --format FORMAT         Specify HDF format: hdf4 or hdf5 (default: hdf4)\n"
//               << "  -s, --subdirs SUBDIRS       Comma-separated list of subdirectories under --dir\n"
//               << "                              scans each <dir>/<subdir> (e.g. -s 2011,2012)\n"
//               << "  -m, --mask FILE             Specify a mask file (only HDF4 currently supported)\n"
//               << "  -V, --values VALUES         Comma-separated list of mask values (default: 1) to extract from datasets\n"
//               << "  -l, --lower VALUE           Exclude values less than VALUE from analysis and stats (default: -9999)\n"
//               << "  -u, --upper VALUE           Exclude values greater than VALUE from analysis and stats (default: 20000)\n"
//               << "  -r, --ranges VALUE          Specify the valid ranges for each dataset, otherwise use values from -u -l\n"
//               << "                              format is DS:VMIN:VMAX comma separated (e.g. NDVI:-2000:10000,ET:0.1:20)\n"
//               << "  -o, --output FILE           Output CSV file to save the time series statistics\n"
//               << "  -v, --version               Show version information\n"
//               << "  -h, --help                  Show this help message\n";
// }

// void print_version() {
//     std::cout << "ts - time series utilities for HDF4 and HDF5 data, version 1.0\n"
//               << "\nCopyright (C) 2025 Eduardo Jimenez Hernandez <eduardojh@arizona.edu>.\n"
//               << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
//               << "This is free software: you are free to change and redistribute it.\n"
//               << "There is NO WARRANTY, to the extent permitted by law.\n";
// }

// std::string get_hdf4_type_name(int32_t data_type) {
//     // Warning: doublecheck
//     switch (data_type) {
//         case DFNT_CHAR:        return "DFNT_CHAR";
//         case DFNT_UCHAR:       return "DFNT_UCHAR";
//         case DFNT_INT8:        return "DFNT_INT8";
//         case DFNT_UINT8:       return "DFNT_UINT8";
//         case DFNT_INT16:       return "DFNT_INT16";
//         case DFNT_UINT16:      return "DFNT_UINT16";
//         case DFNT_INT32:       return "DFNT_INT32";
//         case DFNT_UINT32:      return "DFNT_UINT32";
//         case DFNT_FLOAT32:     return "DFNT_FLOAT32";
//         case DFNT_FLOAT64:     return "DFNT_FLOAT64";
//         case DFNT_NINT8:       return "DFNT_NINT8";
//         case DFNT_NUINT8:      return "DFNT_NUINT8";
//         case DFNT_NINT16:      return "DFNT_NINT16";
//         case DFNT_NUINT16:     return "DFNT_NUINT16";
//         case DFNT_NINT32:      return "DFNT_NINT32";
//         case DFNT_NUINT32:     return "DFNT_NUINT32";
//         case DFNT_NFLOAT32:    return "DFNT_NFLOAT32";
//         case DFNT_NFLOAT64:    return "DFNT_NFLOAT64";
//         default:               return "UNKNOWN_TYPE";
//     }
// }

// std::vector<std::string> read_hdf4_info(const std::string& file_path) {
//     std::vector<std::string> dataset_names;

//     int32_t sd_id = SDstart(file_path.c_str(), DFACC_READ);
//     if (sd_id < 0) {
//         std::cerr << "Failed to open HDF4 file: " << file_path << "\n";
//         return dataset_names;
//     }

//     int32_t num_datasets = 0, num_file_attrs = 0;
//     if (SDfileinfo(sd_id, &num_datasets, &num_file_attrs) != 0) {
//         std::cerr << "Failed to get HDF4 file info: " << file_path << "\n";
//         SDend(sd_id);
//         return dataset_names;
//     }

//     std::cout << "  HDF4 file contains: "
//               << num_datasets << " datasets, "
//               << num_file_attrs << " file attributes.\n";

//     for (int32_t i = 0; i < num_datasets; ++i) {
//         int32_t sds_id = SDselect(sd_id, i);
//         if (sds_id == -1) continue;

//         char name[H4_MAX_NC_NAME];
//         int32_t rank, data_type, n_attrs;
//         int32_t dimsizes[H4_MAX_VAR_DIMS];

//         if (SDgetinfo(sds_id, name, &rank, dimsizes, &data_type, &n_attrs) == 0) {
//             dataset_names.emplace_back(name);

//             std::cout << "  " << i + 1 << " → " << name << " [";
//             for (int j = 0; j < rank; ++j) {
//                 std::cout << dimsizes[j];
//                 if (j < rank - 1) std::cout << " x ";
//             }
//             std::cout << "] TYPE=" << get_hdf4_type_name(data_type) << "(" << data_type << ")\n";
//         }

//         SDendaccess(sds_id);
//     }

//     SDend(sd_id);
//     return dataset_names;
// }

// std::vector<int16_t> read_mask_dataset_hdf4(const std::string& mask_path) {

//     // Reads a HDF4 file with the mask values to extract from the time series
//     // Masks usually have a single dataset of type int16

//     std::cout << "Openning file mask: " << mask_path << "\n";

//     std::vector<int16_t> empty;

//     int32_t sd_id_mask = SDstart(mask_path.c_str(), DFACC_READ);
//     if (sd_id_mask == -1) {
//         std::cerr << "Failed to open HDF4 mask file: " << mask_path << "\n";
//         return empty;
//     }

//     int32_t mask_datasets = 0, mask_file_attrs = 0;
//     if (SDfileinfo(sd_id_mask, &mask_datasets, &mask_file_attrs) != 0) {
//         std::cerr << "Failed to get file info from mask.\n";
//         SDend(sd_id_mask);
//         return empty;
//     }

//     std::cout << "Mask datasets: " << mask_datasets << "\n";

//     // A single mask dataset is expected
//     int j = 0;
//     if (mask_datasets == 1) {

//         int32_t sds_id_mask = SDselect(sd_id_mask, j);  // Select the first and only dataset
//         if (sds_id_mask == -1) {
//             std::cerr << "Failed to select mask dataset index " << j << "\n";
//             return empty;
//         }

//         char name_mask[H4_MAX_NC_NAME];
//         int32_t rank_mask, data_type_mask, n_attrs_mask;
//         int32_t dimsizes_mask[H4_MAX_VAR_DIMS];

//         if (SDgetinfo(sds_id_mask, name_mask, &rank_mask, dimsizes_mask, &data_type_mask, &n_attrs_mask) != 0) {
//             std::cerr << "Failed to get info for mask dataset index " << j << "\n";
//             SDendaccess(sds_id_mask);
//             return empty;
//         }

//         std::cout << "  Mask dataset " << j << ": '" << name_mask << "'";

//         if (rank_mask != 2) {
//             std::cout << " is not 2D. Skipping.\n";
//             SDendaccess(sds_id_mask);
//             return empty;
//         }

//         int mask_rows = dimsizes_mask[0];
//         int mask_cols = dimsizes_mask[1];
//         std::cout << " [" << mask_rows << "x" << mask_cols << "]";

//         // Values for display
//         int rmax_mask = std::min(mask_rows, 5);
//         int cmax_mask = std::min(mask_cols, 5);

//         // Read all the rows and columns
//         int32_t mask_start[2] = {0, 0};
//         int32_t mask_edges[2] = {mask_rows, mask_cols};
//         std::string type_name = get_hdf4_type_name(data_type_mask);

//         if (data_type_mask == DFNT_INT16) {
//             std::vector<int16_t> mask(mask_rows * mask_cols);
//             if (SDreaddata(sds_id_mask, mask_start, nullptr, mask_edges, mask.data()) == 0) {
//                 std::cout << ", Type: INT16 (" << type_name << ") preview: [" << rmax_mask << "x" << cmax_mask << "]\n";
//                 for (int r = 0; r < rmax_mask; ++r) {
//                     for (int c = 0; c < cmax_mask; ++c) {
//                         std::cout << std::setw(8) << mask[r * mask_cols + c];
//                     }
//                     std::cout << "\n";
//                 }
//                 SDendaccess(sds_id_mask);
//                 SDend(sd_id_mask);
//                 std::cout << "\n";
//                 return mask;
//             } else {
//                 std::cerr << "\nFailed to read INT16 (" << type_name << ") data.\n";
//                 SDend(sd_id_mask);
//                 return empty;
//             }
//         } else {
//             std::cout << ", Type: " << data_type_mask << " is unsupported for mask, only INT16 (" << type_name << ")\n";
//             SDend(sd_id_mask);
//             return empty;
//         }
//     }
    
//     // Else, close and return empty
//     std::cout << "Invalid mask format";
//     SDend(sd_id_mask);
//     return empty;
// }

// template <typename T>
// void compute_stats(const std::vector<T>& data,
//                    T& min_val,
//                    T& max_val,
//                    float& mean,
//                    float& stddev
//                    ) {
//     if (data.empty()) {
//         std::cerr << "Vector is empty. Cannot compute statistics.\n";
//         return;
//     }

//     min_val = std::numeric_limits<T>::max();
//     max_val = std::numeric_limits<T>::lowest();  // works for signed/unsigned
//     float sum = 0.0;
//     float sum_sq = 0.0;

//     for (const T& val : data) {
//         if (val < min_val) min_val = val;
//         if (val > max_val) max_val = val;

//         sum += static_cast<float>(val);
//         sum_sq += static_cast<float>(val) * val;
//     }

//     size_t n = data.size();
//     mean = sum / n;
//     stddev = std::sqrt((sum_sq / n) - (mean * mean));
// }

// void read_hdf4_file_create_csv(const std::string& file_path,
//                           std::vector<int16_t>& mask,
//                           std::vector<int16_t>& mask_values,
//                           const float& vmin,
//                           const float& vmax,
//                           const std::string& csv_file,
//                           const std::string& date_str,
//                           const std::map<std::string, std::pair<float, float>>& dataset_ranges) {
//     // Reads all the datasets in a HDF4 file and creates an entry in the CSV time series file

//     int32_t sd_id = SDstart(file_path.c_str(), DFACC_READ);
//     if (sd_id == -1) {
//         std::cerr << "Failed to open HDF4 file: " << file_path << "\n";
//         return;
//     }

//     int32_t num_datasets = 0, num_file_attrs = 0;
//     if (SDfileinfo(sd_id, &num_datasets, &num_file_attrs) != 0) {
//         std::cerr << "Failed to get file info.\n";
//         SDend(sd_id);
//         return;
//     }

//     // Write the date to csv file
//     std::ofstream out(csv_file, std::ios::app);  // open in append mode
//     if (!out.is_open()) {
//         std::cerr << "Failed to open CSV file: " << csv_file << "\n";
//         return;
//     }
//     out << date_str << ",";
//     out.close();  // ensure flush and release

//     for (int i = 0; i < num_datasets; ++i) {
//         int32_t sds_id = SDselect(sd_id, i);
//         if (sds_id == -1) {
//             std::cerr << "Failed to select dataset index " << i << "\n";
//             continue;
//         }

//         char name[H4_MAX_NC_NAME];
//         int32_t rank, data_type, n_attrs;
//         int32_t dimsizes[H4_MAX_VAR_DIMS];

//         if (SDgetinfo(sds_id, name, &rank, dimsizes, &data_type, &n_attrs) != 0) {
//             std::cerr << "Failed to get info for dataset index " << i << "\n";
//             SDendaccess(sds_id);
//             continue;
//         }

//         std::cout << "  Dataset " << i << ": '" << name << "'";

//         if (rank != 2) {
//             std::cout << " is not 2D. Skipping.\n";
//             SDendaccess(sds_id);
//             continue;
//         }

//         int rows = dimsizes[0];
//         int cols = dimsizes[1];
//         std::cout << " [" << rows << "x" << cols << "]";

//         // Values for display
//         int rmax = std::min(rows, 5);
//         int cmax = std::min(cols, 5);

//         // Read all the rows and columns
//         int32_t start[2] = {0, 0};
//         int32_t edges[2] = {rows, cols};

//         // Use a valid range [lower_bound, upper_bound] to ignore outliers
//         float lower_bound, upper_bound;

//         // Get the valid range for each dataset
//         auto it = dataset_ranges.find(name);
//         if (it != dataset_ranges.end()) {
//             lower_bound = it->second.first;
//             upper_bound = it->second.second;
//         } else {
//             // Fallback if not found in the map (use default range)
//             std::cout <<  "  Dataset: " << name << " not found, using default range.";
//             lower_bound = vmin;
//             upper_bound = vmax;
//         }

//         switch (data_type) {
//             case DFNT_INT8: {
//                 std::vector<int8_t> buffer(rows * cols);
//                 if (SDreaddata(sds_id, start, nullptr, edges, buffer.data()) == 0) {
//                     std::cout << ", Type: INT8 preview: [" << rmax << "x" << cmax << "]\n";
//                     for (int r = 0; r < rmax; ++r) {
//                         for (int c = 0; c < cmax; ++c) {
//                             std::cout << std::setw(6) << static_cast<int>(buffer[r * cols + c]);
//                         }
//                         std::cout << "\n";
//                     }
                    
//                     // Iterate over the mask values
//                     for (const int16_t& mask_value : mask_values) {
//                         std::cout << "  --mask_value=" << mask_value ;

//                         // Store all the values from the dataset at the same position
//                         std::vector<int8_t> data_values;
//                         int8_t min_val, max_val;
//                         float mean_val, stddev;
                       
//                         // Find the position of the mask_value in the mask
//                         for (size_t val_pos=0; val_pos < mask.size(); ++val_pos) {
//                             if (mask[val_pos] == mask_value) {
//                                 // std::cout  << val_pos << ","; // show the position

//                                 // Discard out-of-range values
//                                 if (buffer[val_pos] > upper_bound || buffer[val_pos] < lower_bound) {
//                                     std::cout << "  **Outlier value ignored: " << buffer[val_pos] << " outside (" << lower_bound << "," << upper_bound << ")\n";
//                                     continue;
//                                 }

//                                 // Store the data values found at this position
//                                 data_values.push_back(buffer[val_pos]);
//                             }
//                         }

//                         // Compute and display stats
//                         compute_stats(data_values, min_val, max_val, mean_val, stddev);
//                         std::cout << " min: " << min_val << ", max: " << max_val
//                                   << ", mean: " << mean_val << ", stddev: " << stddev
//                                   << ", npixels: " << data_values.size() << "\n";
                        
//                         // Save to csv file
//                         std::ofstream out(csv_file, std::ios::app);  // open in append mode
//                         if (!out.is_open()) {
//                             std::cerr << "Failed to open CSV file: " << csv_file << "\n";
//                             return;
//                         }
//                         out << min_val << "," << max_val << ","
//                             << mean_val << "," << stddev << ","
//                             << data_values.size() << ",";
//                         out.close();  // ensure flush and release

//                     }
//                 } else {
//                     std::cerr << "\nFailed to read INT8 data.\n";
//                 }
//                 break;
//             }

//             case DFNT_INT16: {
//                 std::vector<int16_t> buffer(rows * cols);
//                 if (SDreaddata(sds_id, start, nullptr, edges, buffer.data()) == 0) {
//                     std::cout << ", Type: INT16 preview: [" << rmax << "x" << cmax << "]\n";
//                     for (int r = 0; r < rmax; ++r) {
//                         for (int c = 0; c < cmax; ++c) {
//                             std::cout << std::setw(8) << buffer[r * cols + c];
//                         }
//                         std::cout << "\n";
//                     }

//                     // Iterate over the mask values
//                     for (const int16_t& mask_value : mask_values) {
//                         std::cout << "  --mask_value=" << mask_value ;

//                         // Store all the values from the dataset at the same position
//                         std::vector<int16_t> data_values;
//                         int16_t min_val, max_val;
//                         float mean_val, stddev;
                        
//                         // Find the position of the mask_value in the mask
//                         for (size_t val_pos=0; val_pos < mask.size(); ++val_pos) {
//                             if (mask[val_pos] == mask_value) {
//                                 // std::cout  << val_pos << ","; // show the position

//                                 // Discard out-of-range values
//                                 if (buffer[val_pos] > upper_bound || buffer[val_pos] < lower_bound) {
//                                     std::cout << "  **Outlier value ignored: " << buffer[val_pos] << " outside (" << lower_bound << "," << upper_bound << ")\n";
//                                     continue;
//                                 }

//                                 // Store the data values found at this position
//                                 data_values.push_back(buffer[val_pos]);
//                             }
//                         }

//                         // Compute and display stats
//                         compute_stats(data_values, min_val, max_val, mean_val, stddev);
//                         std::cout << " min: " << min_val << ", max: " << max_val
//                                   << ", mean: " << mean_val << ", stddev: " << stddev
//                                   << ", npixels: " << data_values.size() << "\n";
                        
//                         // Save to csv file
//                         std::ofstream out(csv_file, std::ios::app);  // open in append mode
//                         if (!out.is_open()) {
//                             std::cerr << "Failed to open CSV file: " << csv_file << "\n";
//                             return;
//                         }
//                         out << min_val << "," << max_val << ","
//                             << mean_val << "," << stddev << ","
//                             << data_values.size() << ",";
//                         out.close();  // ensure flush and release

//                     }
//                 } else {
//                     std::cerr << "\nFailed to read INT16 data.\n";
//                 }
//                 break;
//             }

//             case DFNT_FLOAT32: {
//                 std::vector<float> buffer(rows * cols);
//                 if (SDreaddata(sds_id, start, nullptr, edges, buffer.data()) == 0) {
//                     std::cout << ", Type: FLOAT32 preview: [" << rmax << "x" << cmax << "]\n";
//                     for (int r = 0; r < rmax; ++r) {
//                         for (int c = 0; c < cmax; ++c) {
//                             std::cout << std::setw(10) << std::fixed << std::setprecision(4)
//                                       << buffer[r * cols + c];
//                         }
//                         std::cout << "\n";
//                     }

//                     // Iterate over the mask values
//                     for (const int16_t& mask_value : mask_values) {
//                         std::cout << "  --mask_value=" << mask_value ;

//                         // Store all the values from the dataset at the same position
//                         std::vector<float> data_values;
//                         float min_val, max_val;
//                         float mean_val, stddev;
                        
//                         // Find the position of the mask_value in the mask
//                         for (size_t val_pos=0; val_pos < mask.size(); ++val_pos) {
//                             if (mask[val_pos] == mask_value) {
//                                 // std::cout  << val_pos << ","; // show the position

//                                 // Discard out-of-range values
//                                 if (buffer[val_pos] > upper_bound || buffer[val_pos] < lower_bound) {
//                                     std::cout << "  **Outlier value ignored: " << buffer[val_pos] << " outside (" << lower_bound << "," << upper_bound << ")\n";
//                                     continue;
//                                 }

//                                 // Store the data values found at this position
//                                 data_values.push_back(buffer[val_pos]);
//                             }
//                         }

//                         // Compute and display stats
//                         compute_stats(data_values, min_val, max_val, mean_val, stddev);
//                         std::cout << " min: " << min_val << ", max: " << max_val
//                                   << ", mean: " << mean_val << ", stddev: " << stddev
//                                   << ", npixels: " << data_values.size() << "\n";
                        
//                         // Save to csv file
//                         std::ofstream out(csv_file, std::ios::app);  // open in append mode
//                         if (!out.is_open()) {
//                             std::cerr << "Failed to open CSV file: " << csv_file << "\n";
//                             return;
//                         }
//                         out << min_val << "," << max_val << ","
//                             << mean_val << "," << stddev << ","
//                             << data_values.size() << ",";
//                         out.close();  // ensure flush and release

//                     }
//                 } else {
//                     std::cerr << "\nFailed to read FLOAT32 data.\n";
//                 }
//                 break;
//             }
//             default:
//                 std::cout << ", Type: " << data_type << " (unsupported preview type)\n";
//         }
//         SDendaccess(sds_id);
//     }

//     // Add a return/new line char to the csv file after each HDF file
//     out.open(csv_file, std::ios::app);  // open in append mode
//     if (!out.is_open()) {
//         std::cerr << "Failed to reopen CSV file: " << csv_file << "\n";
//         return;
//     }
//     out << "\n";
//     out.close();  // ensure flush and release

//     SDend(sd_id);
// }

// void read_hdf5_info(const std::string& file_path) {
//     hid_t file_id = H5Fopen(file_path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
//     if (file_id < 0) {
//         std::cerr << "Failed to open HDF5 file: " << file_path << "\n";
//         return;
//     }

//     std::cout << "Opened HDF5 file successfully: " << file_path << "\n";
//     H5Fclose(file_id);
// }

// std::vector<std::string> split_list(const std::string& str) {
//     // Splits a list of comma separated strings into a vector
//     std::vector<std::string> result;
//     std::stringstream ss(str);
//     std::string token;
//     while (std::getline(ss, token, ',')) {
//         if (!token.empty()) result.push_back(token);
//     }
//     return result;
// }

// std::vector<int16_t> split_list_int(const std::string& str) {
//     // Splits a list of comma separated strings into a vector of int16
//     std::vector<int16_t> result;
//     std::stringstream ss(str);
//     std::string token;
//     while (std::getline(ss, token, ',')) {
//         if (!token.empty()) result.push_back(std::stoi(token));
//     }
//     return result;
// }

// std::string convert_doy_date(const std::string& filename) {
//     // This will look for the pattern "A2022007" in the file name 
//     // and separate the year and day-of-year (YYYYDOY)
//     std::regex date_regex(R"(A(\d{4})(\d{3}))");
//     std::smatch match;

//     if (std::regex_search(filename, match, date_regex)) {
//         int year = std::stoi(match[1].str());
//         int doy = std::stoi(match[2].str());

//         // Convert year + DOY to YYYY-MM-DD
//         std::tm time = {};
//         time.tm_year = year - 1900;
//         time.tm_mday = doy;

//         // mktime advances from Jan 1 by tm_mday days
//         std::mktime(&time);

//         std::ostringstream oss;
//         oss << std::put_time(&time, "%Y-%m-%d");
//         return oss.str();
//     }

//     return "INVALID_DATE";
// }

// fs::path find_first_valid_file(const std::string& root,
//                                const std::string& extension,
//                                const std::vector<std::string>& subdirs) {
//     namespace fs = std::filesystem;

//     if (subdirs.empty()) {
//         // Case: search in the root directory only
//         for (const auto& entry : fs::directory_iterator(root)) {
//             if (entry.is_regular_file() && entry.path().extension() == extension) {
//                 return entry.path();
//             }
//         }
//     } else {
//         // Case: search in each subdir under root
//         for (const auto& sub : subdirs) {
//             fs::path path = fs::path(root) / sub;
//             if (!fs::exists(path)) continue;

//             for (const auto& entry : fs::directory_iterator(path)) {
//                 if (entry.is_regular_file() && entry.path().extension() == extension) {
//                     return entry.path();
//                 }
//             }
//         }
//     }

//     return fs::path();  // no file found
// }

// // EVEN NEWER FUNCTION, TESTING STILL
// void scan_directory_tree(const std::string& root,
//                          const std::string& format,
//                          const std::vector<std::string>& subdirs,
//                          const std::string& mask_path,
//                          std::vector<int16_t> mask_values,
//                          const float& vmin,
//                          const float& vmax,
//                          const std::string& csv_file,
//                          const std::map<std::string, std::pair<float, float>>& dataset_ranges) {
//     // Function to scan the directories root only, root+subdirs
//     // Updated to create and initialize the CSV file with the headers 
//     namespace fs = std::filesystem;
//     std::string extension = (format == "hdf4") ? ".hdf" : ".h5";
//     int file_count = 0;
//     // bool header_written = false;

//     // if (subdirs.empty() && !subsubdirs.empty()) {
//     //     std::cout << "Warning: subsubdirs provided but no subdirs; they will be ignored.\n";
//     // }

//     // Open the mask once
//     std::vector<int16_t> mask;
//     if (format == "hdf4") {
//         mask = read_mask_dataset_hdf4(mask_path);
//     } else {
//         std::cerr << "HDF5 mask handling not implemented yet.\n";
//         return;
//     }

//     // Find the first file to get the dataset names and initialize the CSV file with headers
//     fs::path first_file = find_first_valid_file(root, extension, subdirs);

//     if (first_file.empty()) {
//         std::cout << "No " << extension << " files found to write CSV header.\n";
//         return;
//     }

//     std::vector<std::string> dataset_names = read_hdf4_info(first_file);

//     // Write header to the CSV file
//     std::ofstream out(csv_file, std::ios::out | std::ios::trunc);
//     if (!out.is_open()) {
//         std::cerr << "Failed to open CSV file: " << csv_file << "\n";
//         return;
//     }

//     out << "Date,";
//     for (const auto& name : dataset_names) {
//         for (const int16_t& mask_val : mask_values) {
//             out << name << "_min_" << mask_val << ",";
//             out << name << "_max_" << mask_val << ",";
//             out << name << "_avg_" << mask_val << ",";
//             out << name << "_stdev_" << mask_val << ",";
//             out << name << "_npixels_" << mask_val << ",";
//         }
//     }
//     out << "\n";
//     out.close();

//     // Lambda to process one directory
//     auto process_directory = [&](const fs::path& dir_path) {
//         if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) return;

//         std::vector<fs::directory_entry> files;
//         for (const auto& entry : fs::directory_iterator(dir_path)) {
//             if (entry.is_regular_file() && entry.path().extension() == extension) {
//                 files.push_back(entry);
//             }
//         }

//         std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
//             return a.path().filename() < b.path().filename();
//         });

//         if (files.empty()) {
//             std::cout << "No " << extension << " files found in: " << dir_path << "\n";
//             return;
//         }

//         for (const auto& entry : files) {
//             std::string filename = entry.path().filename().string();
//             std::string date_str = convert_doy_date(filename);

//             std::cout << "  Found file: " << filename << ", Date: " << date_str << "\n";

//             ++file_count;
//             if (format == "hdf5") {
//                 read_hdf5_info(entry.path().string()); // placeholder
//             } else {
//                 read_hdf4_file_create_csv(entry.path().string(), mask, mask_values, vmin, vmax, csv_file, date_str, dataset_ranges);
//             }
//         }
//     };

//     // Case 1: no subdirs
//     if (subdirs.empty()) {
//         std::cout << "Scanning root directory: " << root << "\n";
//         process_directory(root);
//     } else {
//         for (const auto& sub : subdirs) {
//             fs::path target = fs::path(root) / sub;
//             std::cout << "Scanning: " << target << "\n";
//             process_directory(target);
//         }
//     }

//     if (file_count == 0) {
//         std::cout << "No " << extension << " files found in any directory.\n";
//     }

//     if (file_count == 0) {
//         std::cout << "No " << extension << " files found in any directory.\n";
//     }
// }

int main(int argc, char* argv[]) {
    std::string directory;
    std::string csv_file;
    std::string format = "hdf4";  // default to hdf4
    std::vector<std::string> subdirs;
    std::string mask_file_path;
    std::vector<int16_t> mask_values = {1};
    float vmin = -9999;
    float vmax = 20000;
    std::map<std::string, std::pair<float, float>> dataset_ranges;

    // IMPORTANT: when addding a new option, don't forget to
    // update the short option, followed by colon if the
    // argument is required.
    const char* const short_opts = "hvd:f:s:m:V:l:u:o:r:";
    
    const option long_opts[] = {
        {"help",       no_argument,       nullptr, 'h'},
        {"format",     required_argument, nullptr, 'f'},
        {"dir",        required_argument, nullptr, 'd'},
        {"subdirs",    required_argument, nullptr, 's'},
        {"version",    no_argument,       nullptr, 'v'},
        {"mask",       required_argument, nullptr, 'm'},
        {"values",     required_argument, nullptr, 'V'},
        {"lower",      required_argument, nullptr, 'l'},
        {"upper",      required_argument, nullptr, 'u'},
        {"output",     required_argument, nullptr, 'o'},
       {"ranges",     required_argument, nullptr, 'r'},
       {nullptr,      0,                 nullptr,  0 }
    };

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (opt == -1)
            break;

        switch (opt) {
            case 'h':
                print_help();
                return EXIT_SUCCESS;
            case 'v':
                print_version();
                return EXIT_SUCCESS;
            case 'd': // directory option
                directory = optarg;
                break;
            case 'm': // mask path
                mask_file_path = optarg;
                break;
            case 'f':
                format = optarg;
                if (format != "hdf4" && format != "hdf5") {
                    std::cerr << "Invalid format: " << format << "\n";
                    print_help();
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                subdirs = split_list(optarg);
                break;
            case 'V':
                mask_values = split_list_int(optarg);
                break;
            case 'l':
                vmin = std::stof(std::string(optarg));
                break;
            case 'u':
                vmax = std::stof(std::string(optarg));
                break;
            case 'o':
                csv_file = optarg;
                break;
            case 'r': {
                std::string range_arg = optarg;
                std::stringstream ss(range_arg);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    size_t colon1 = token.find(':');
                    size_t colon2 = token.rfind(':');
                    if (colon1 == std::string::npos || colon2 == std::string::npos || colon1 == colon2) {
                        std::cerr << "Invalid range format: " << token << "\n";
                        continue;
                    }
                    std::string name = token.substr(0, colon1);
                    float rmin = std::stof(token.substr(colon1 + 1, colon2 - colon1 - 1));
                    float rmax = std::stof(token.substr(colon2 + 1));
                    dataset_ranges[name] = std::make_pair(rmin, rmax);
                }
                break;
            }
            case '?': // Unrecognized option
            default:
                print_help();
                return EXIT_FAILURE;
        }
    }

    if (directory.empty()) {
        std::cerr << "Error: Directory path is required.\n\n";
        print_help();
        return EXIT_FAILURE;
    }

    if (mask_file_path.empty()) {
        std::cerr << "Error: Mask file path is required.\n\n";
        print_help();
        return EXIT_FAILURE;
    }

    if (csv_file.empty()) {
        std::cerr << "Error: Output CSV file path is required.\n\n";
        print_help();
        return EXIT_FAILURE;
    }

    std::cout << "\nTime series extraction from HDF files.\n";
    std::cout << "Format: " << format << "\n";
    std::cout << "Scanning directory: " << directory << "\n";
    std::cout << "Including sub-directories: " << std::boolalpha << !subdirs.empty() << " ";
    for (size_t i=0; i < subdirs.size(); i++) {
        std::cout << subdirs[i];
        if (i != subdirs.size()-1)
            std::cout << ",";
    }
    std::cout << "\n";
    std::cout << "Using mask file: " << mask_file_path << "\n";
    std::cout << "Using mask values (default: 1): ";
    for (size_t i=0; i < mask_values.size(); i++) {
        std::cout << mask_values[i];
        if (i != mask_values.size()-1)
            std::cout << ",";
    }
    std::cout << "\n";
    std::cout << "Lower value filter: " << vmin << "\n";
    std::cout << "Upper value filter: " << vmax << "\n";
    std::cout << "Output (CSV) file: " << csv_file << "\n";
    std::cout << "\n\n";

    // scan_directory(directory, format);
    scan_directory_tree(directory, format, subdirs, 
                        mask_file_path, mask_values,
                        vmin, vmax, csv_file, dataset_ranges);
    std::cout << "Ice never dies!\n";
    return EXIT_SUCCESS;
}