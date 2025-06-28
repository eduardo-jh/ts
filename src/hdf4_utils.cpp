#include "hdf4_utils.hpp"

// namespace fs = std::filesystem;

std::string get_hdf4_type_name(int32_t data_type) {
    // Warning: doublecheck
    switch (data_type) {
        case DFNT_CHAR:        return "DFNT_CHAR";
        case DFNT_UCHAR:       return "DFNT_UCHAR";
        case DFNT_INT8:        return "DFNT_INT8";
        case DFNT_UINT8:       return "DFNT_UINT8";
        case DFNT_INT16:       return "DFNT_INT16";
        case DFNT_UINT16:      return "DFNT_UINT16";
        case DFNT_INT32:       return "DFNT_INT32";
        case DFNT_UINT32:      return "DFNT_UINT32";
        case DFNT_FLOAT32:     return "DFNT_FLOAT32";
        case DFNT_FLOAT64:     return "DFNT_FLOAT64";
        case DFNT_NINT8:       return "DFNT_NINT8";
        case DFNT_NUINT8:      return "DFNT_NUINT8";
        case DFNT_NINT16:      return "DFNT_NINT16";
        case DFNT_NUINT16:     return "DFNT_NUINT16";
        case DFNT_NINT32:      return "DFNT_NINT32";
        case DFNT_NUINT32:     return "DFNT_NUINT32";
        case DFNT_NFLOAT32:    return "DFNT_NFLOAT32";
        case DFNT_NFLOAT64:    return "DFNT_NFLOAT64";
        default:               return "UNKNOWN_TYPE";
    }
}

std::vector<std::string> read_hdf4_info(const std::string& file_path) {
    std::vector<std::string> dataset_names;

    int32_t sd_id = SDstart(file_path.c_str(), DFACC_READ);
    if (sd_id < 0) {
        std::cerr << "Failed to open HDF4 file: " << file_path << "\n";
        return dataset_names;
    }

    int32_t num_datasets = 0, num_file_attrs = 0;
    if (SDfileinfo(sd_id, &num_datasets, &num_file_attrs) != 0) {
        std::cerr << "Failed to get HDF4 file info: " << file_path << "\n";
        SDend(sd_id);
        return dataset_names;
    }

    std::cout << "  HDF4 file contains: "
              << num_datasets << " datasets, "
              << num_file_attrs << " file attributes.\n";

    for (int32_t i = 0; i < num_datasets; ++i) {
        int32_t sds_id = SDselect(sd_id, i);
        if (sds_id == -1) continue;

        char name[H4_MAX_NC_NAME];
        int32_t rank, data_type, n_attrs;
        int32_t dimsizes[H4_MAX_VAR_DIMS];

        if (SDgetinfo(sds_id, name, &rank, dimsizes, &data_type, &n_attrs) == 0) {
            dataset_names.emplace_back(name);

            std::cout << "  " << i + 1 << " → " << name << " [";
            for (int j = 0; j < rank; ++j) {
                std::cout << dimsizes[j];
                if (j < rank - 1) std::cout << " x ";
            }
            std::cout << "] TYPE=" << get_hdf4_type_name(data_type) << "(" << data_type << ")\n";
        }

        SDendaccess(sds_id);
    }

    SDend(sd_id);
    return dataset_names;
}

std::vector<int16_t> read_mask_dataset_hdf4(const std::string& mask_path) {

    // Reads a HDF4 file with the mask values to extract from the time series
    // Masks usually have a single dataset of type int16

    std::cout << "Openning file mask: " << mask_path << "\n";

    std::vector<int16_t> empty;

    int32_t sd_id_mask = SDstart(mask_path.c_str(), DFACC_READ);
    if (sd_id_mask == -1) {
        std::cerr << "Failed to open HDF4 mask file: " << mask_path << "\n";
        return empty;
    }

    int32_t mask_datasets = 0, mask_file_attrs = 0;
    if (SDfileinfo(sd_id_mask, &mask_datasets, &mask_file_attrs) != 0) {
        std::cerr << "Failed to get file info from mask.\n";
        SDend(sd_id_mask);
        return empty;
    }

    std::cout << "Mask datasets: " << mask_datasets << "\n";

    // A single mask dataset is expected
    int j = 0;
    if (mask_datasets == 1) {

        int32_t sds_id_mask = SDselect(sd_id_mask, j);  // Select the first and only dataset
        if (sds_id_mask == -1) {
            std::cerr << "Failed to select mask dataset index " << j << "\n";
            return empty;
        }

        char name_mask[H4_MAX_NC_NAME];
        int32_t rank_mask, data_type_mask, n_attrs_mask;
        int32_t dimsizes_mask[H4_MAX_VAR_DIMS];

        if (SDgetinfo(sds_id_mask, name_mask, &rank_mask, dimsizes_mask, &data_type_mask, &n_attrs_mask) != 0) {
            std::cerr << "Failed to get info for mask dataset index " << j << "\n";
            SDendaccess(sds_id_mask);
            return empty;
        }

        std::cout << "  Mask dataset " << j << ": '" << name_mask << "'";

        if (rank_mask != 2) {
            std::cout << " is not 2D. Skipping.\n";
            SDendaccess(sds_id_mask);
            return empty;
        }

        int mask_rows = dimsizes_mask[0];
        int mask_cols = dimsizes_mask[1];
        std::cout << " [" << mask_rows << "x" << mask_cols << "]";

        // Values for display
        int rmax_mask = std::min(mask_rows, 5);
        int cmax_mask = std::min(mask_cols, 5);

        // Read all the rows and columns
        int32_t mask_start[2] = {0, 0};
        int32_t mask_edges[2] = {mask_rows, mask_cols};
        std::string type_name = get_hdf4_type_name(data_type_mask);

        if (data_type_mask == DFNT_INT16) {
            std::vector<int16_t> mask(mask_rows * mask_cols);
            if (SDreaddata(sds_id_mask, mask_start, nullptr, mask_edges, mask.data()) == 0) {
                std::cout << ", Type: INT16 (" << type_name << ") preview: [" << rmax_mask << "x" << cmax_mask << "]\n";
                for (int r = 0; r < rmax_mask; ++r) {
                    for (int c = 0; c < cmax_mask; ++c) {
                        std::cout << std::setw(8) << mask[r * mask_cols + c];
                    }
                    std::cout << "\n";
                }
                SDendaccess(sds_id_mask);
                SDend(sd_id_mask);
                std::cout << "\n";
                return mask;
            } else {
                std::cerr << "\nFailed to read INT16 (" << type_name << ") data.\n";
                SDend(sd_id_mask);
                return empty;
            }
        } else {
            std::cout << ", Type: " << data_type_mask << " is unsupported for mask, only INT16 (" << type_name << ")\n";
            SDend(sd_id_mask);
            return empty;
        }
    }
    
    // Else, close and return empty
    std::cout << "Invalid mask format";
    SDend(sd_id_mask);
    return empty;
}

void read_hdf4_file_create_csv(const std::string& file_path,
                          std::vector<int16_t>& mask,
                          std::vector<int16_t>& mask_values,
                          const float& vmin,
                          const float& vmax,
                          const std::string& csv_file,
                          const std::string& date_str,
                          const std::map<std::string, std::pair<float, float>>& dataset_ranges) {
    // Reads all the datasets in a HDF4 file and creates an entry in the CSV time series file

    int32_t sd_id = SDstart(file_path.c_str(), DFACC_READ);
    if (sd_id == -1) {
        std::cerr << "Failed to open HDF4 file: " << file_path << "\n";
        return;
    }

    int32_t num_datasets = 0, num_file_attrs = 0;
    if (SDfileinfo(sd_id, &num_datasets, &num_file_attrs) != 0) {
        std::cerr << "Failed to get file info.\n";
        SDend(sd_id);
        return;
    }

    // Write the date to csv file
    std::ofstream out(csv_file, std::ios::app);  // open in append mode
    if (!out.is_open()) {
        std::cerr << "Failed to open CSV file: " << csv_file << "\n";
        return;
    }
    out << date_str << ",";
    out.close();  // ensure flush and release

    for (int i = 0; i < num_datasets; ++i) {
        int32_t sds_id = SDselect(sd_id, i);
        if (sds_id == -1) {
            std::cerr << "Failed to select dataset index " << i << "\n";
            continue;
        }

        char name[H4_MAX_NC_NAME];
        int32_t rank, data_type, n_attrs;
        int32_t dimsizes[H4_MAX_VAR_DIMS];

        if (SDgetinfo(sds_id, name, &rank, dimsizes, &data_type, &n_attrs) != 0) {
            std::cerr << "Failed to get info for dataset index " << i << "\n";
            SDendaccess(sds_id);
            continue;
        }

        std::cout << "  Dataset " << i << ": '" << name << "'";

        if (rank != 2) {
            std::cout << " is not 2D. Skipping.\n";
            SDendaccess(sds_id);
            continue;
        }

        int rows = dimsizes[0];
        int cols = dimsizes[1];
        std::cout << " [" << rows << "x" << cols << "]";

        // Values for display
        int rmax = std::min(rows, 5);
        int cmax = std::min(cols, 5);

        // Read all the rows and columns
        int32_t start[2] = {0, 0};
        int32_t edges[2] = {rows, cols};

        // Use a valid range [lower_bound, upper_bound] to ignore outliers
        float lower_bound, upper_bound;

        // Get the valid range for each dataset
        auto it = dataset_ranges.find(name);
        if (it != dataset_ranges.end()) {
            lower_bound = it->second.first;
            upper_bound = it->second.second;
        } else {
            // Fallback if not found in the map (use default range)
            std::cout <<  "  Dataset: " << name << " not found, using default range.";
            lower_bound = vmin;
            upper_bound = vmax;
        }

        switch (data_type) {
            case DFNT_INT8: {
                std::vector<int8_t> buffer(rows * cols);
                if (SDreaddata(sds_id, start, nullptr, edges, buffer.data()) == 0) {
                    std::cout << ", Type: INT8 preview: [" << rmax << "x" << cmax << "]\n";
                    for (int r = 0; r < rmax; ++r) {
                        for (int c = 0; c < cmax; ++c) {
                            std::cout << std::setw(6) << static_cast<int>(buffer[r * cols + c]);
                        }
                        std::cout << "\n";
                    }
                    
                    // Iterate over the mask values
                    for (const int16_t& mask_value : mask_values) {
                        std::cout << "  --mask_value=" << mask_value ;

                        // Store all the values from the dataset at the same position
                        std::vector<int8_t> data_values;
                        int8_t min_val, max_val;
                        float mean_val, stddev;
                       
                        // Find the position of the mask_value in the mask
                        for (size_t val_pos=0; val_pos < mask.size(); ++val_pos) {
                            if (mask[val_pos] == mask_value) {
                                // std::cout  << val_pos << ","; // show the position

                                // Discard out-of-range values
                                if (buffer[val_pos] > upper_bound || buffer[val_pos] < lower_bound) {
                                    std::cout << "  **Outlier value ignored: " << buffer[val_pos] << " outside (" << lower_bound << "," << upper_bound << ")\n";
                                    continue;
                                }

                                // Store the data values found at this position
                                data_values.push_back(buffer[val_pos]);
                            }
                        }

                        // Compute and display stats
                        compute_stats(data_values, min_val, max_val, mean_val, stddev);
                        std::cout << " min: " << min_val << ", max: " << max_val
                                  << ", mean: " << mean_val << ", stddev: " << stddev
                                  << ", npixels: " << data_values.size() << "\n";
                        
                        // Save to csv file
                        std::ofstream out(csv_file, std::ios::app);  // open in append mode
                        if (!out.is_open()) {
                            std::cerr << "Failed to open CSV file: " << csv_file << "\n";
                            return;
                        }
                        out << min_val << "," << max_val << ","
                            << mean_val << "," << stddev << ","
                            << data_values.size() << ",";
                        out.close();  // ensure flush and release

                    }
                } else {
                    std::cerr << "\nFailed to read INT8 data.\n";
                }
                break;
            }

            case DFNT_INT16: {
                std::vector<int16_t> buffer(rows * cols);
                if (SDreaddata(sds_id, start, nullptr, edges, buffer.data()) == 0) {
                    std::cout << ", Type: INT16 preview: [" << rmax << "x" << cmax << "]\n";
                    for (int r = 0; r < rmax; ++r) {
                        for (int c = 0; c < cmax; ++c) {
                            std::cout << std::setw(8) << buffer[r * cols + c];
                        }
                        std::cout << "\n";
                    }

                    // Iterate over the mask values
                    for (const int16_t& mask_value : mask_values) {
                        std::cout << "  --mask_value=" << mask_value ;

                        // Store all the values from the dataset at the same position
                        std::vector<int16_t> data_values;
                        int16_t min_val, max_val;
                        float mean_val, stddev;
                        
                        // Find the position of the mask_value in the mask
                        for (size_t val_pos=0; val_pos < mask.size(); ++val_pos) {
                            if (mask[val_pos] == mask_value) {
                                // std::cout  << val_pos << ","; // show the position

                                // Discard out-of-range values
                                if (buffer[val_pos] > upper_bound || buffer[val_pos] < lower_bound) {
                                    std::cout << "  **Outlier value ignored: " << buffer[val_pos] << " outside (" << lower_bound << "," << upper_bound << ")\n";
                                    continue;
                                }

                                // Store the data values found at this position
                                data_values.push_back(buffer[val_pos]);
                            }
                        }

                        // Compute and display stats
                        compute_stats(data_values, min_val, max_val, mean_val, stddev);
                        std::cout << " min: " << min_val << ", max: " << max_val
                                  << ", mean: " << mean_val << ", stddev: " << stddev
                                  << ", npixels: " << data_values.size() << "\n";
                        
                        // Save to csv file
                        std::ofstream out(csv_file, std::ios::app);  // open in append mode
                        if (!out.is_open()) {
                            std::cerr << "Failed to open CSV file: " << csv_file << "\n";
                            return;
                        }
                        out << min_val << "," << max_val << ","
                            << mean_val << "," << stddev << ","
                            << data_values.size() << ",";
                        out.close();  // ensure flush and release

                    }
                } else {
                    std::cerr << "\nFailed to read INT16 data.\n";
                }
                break;
            }

            case DFNT_FLOAT32: {
                std::vector<float> buffer(rows * cols);
                if (SDreaddata(sds_id, start, nullptr, edges, buffer.data()) == 0) {
                    std::cout << ", Type: FLOAT32 preview: [" << rmax << "x" << cmax << "]\n";
                    for (int r = 0; r < rmax; ++r) {
                        for (int c = 0; c < cmax; ++c) {
                            std::cout << std::setw(10) << std::fixed << std::setprecision(4)
                                      << buffer[r * cols + c];
                        }
                        std::cout << "\n";
                    }

                    // Iterate over the mask values
                    for (const int16_t& mask_value : mask_values) {
                        std::cout << "  --mask_value=" << mask_value ;

                        // Store all the values from the dataset at the same position
                        std::vector<float> data_values;
                        float min_val, max_val;
                        float mean_val, stddev;
                        
                        // Find the position of the mask_value in the mask
                        for (size_t val_pos=0; val_pos < mask.size(); ++val_pos) {
                            if (mask[val_pos] == mask_value) {
                                // std::cout  << val_pos << ","; // show the position

                                // Discard out-of-range values
                                if (buffer[val_pos] > upper_bound || buffer[val_pos] < lower_bound) {
                                    std::cout << "  **Outlier value ignored: " << buffer[val_pos] << " outside (" << lower_bound << "," << upper_bound << ")\n";
                                    continue;
                                }

                                // Store the data values found at this position
                                data_values.push_back(buffer[val_pos]);
                            }
                        }

                        // Compute and display stats
                        compute_stats(data_values, min_val, max_val, mean_val, stddev);
                        std::cout << " min: " << min_val << ", max: " << max_val
                                  << ", mean: " << mean_val << ", stddev: " << stddev
                                  << ", npixels: " << data_values.size() << "\n";
                        
                        // Save to csv file
                        std::ofstream out(csv_file, std::ios::app);  // open in append mode
                        if (!out.is_open()) {
                            std::cerr << "Failed to open CSV file: " << csv_file << "\n";
                            return;
                        }
                        out << min_val << "," << max_val << ","
                            << mean_val << "," << stddev << ","
                            << data_values.size() << ",";
                        out.close();  // ensure flush and release

                    }
                } else {
                    std::cerr << "\nFailed to read FLOAT32 data.\n";
                }
                break;
            }
            default:
                std::cout << ", Type: " << data_type << " (unsupported preview type)\n";
        }
        SDendaccess(sds_id);
    }
    
    // Add a return/new line char to the csv file after each HDF file
    out.open(csv_file, std::ios::app);  // open in append mode
    if (!out.is_open()) {
        std::cerr << "Failed to reopen CSV file: " << csv_file << "\n";
        return;
    }
    out << "\n";
    out.close();  // ensure flush and release

    SDend(sd_id);
}