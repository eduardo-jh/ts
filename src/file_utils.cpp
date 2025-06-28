#include "file_utils.hpp"

std::string convert_doy_date(const std::string& filename) {
    // This will look for the pattern "A2022007" in the file name 
    // and separate the year and day-of-year (YYYYDOY)
    std::regex date_regex(R"(A(\d{4})(\d{3}))");
    std::smatch match;

    if (std::regex_search(filename, match, date_regex)) {
        int year = std::stoi(match[1].str());
        int doy = std::stoi(match[2].str());

        // Convert year + DOY to YYYY-MM-DD
        std::tm time = {};
        time.tm_year = year - 1900;
        time.tm_mday = doy;

        // mktime advances from Jan 1 by tm_mday days
        std::mktime(&time);

        std::ostringstream oss;
        oss << std::put_time(&time, "%Y-%m-%d");
        return oss.str();
    }

    return "INVALID_DATE";
}

fs::path find_first_valid_file(const std::string& root,
                               const std::string& extension,
                               const std::vector<std::string>& subdirs) {
    namespace fs = std::filesystem;

    if (subdirs.empty()) {
        // Case: search in the root directory only
        for (const auto& entry : fs::directory_iterator(root)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                return entry.path();
            }
        }
    } else {
        // Case: search in each subdir under root
        for (const auto& sub : subdirs) {
            fs::path path = fs::path(root) / sub;
            if (!fs::exists(path)) continue;

            for (const auto& entry : fs::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == extension) {
                    return entry.path();
                }
            }
        }
    }

    return fs::path();  // no file found
}

// EVEN NEWER FUNCTION, TESTING STILL
void scan_directory_tree(const std::string& root,
                         const std::string& format,
                         const std::vector<std::string>& subdirs,
                         const std::string& mask_path,
                         std::vector<int16_t> mask_values,
                         const float& vmin,
                         const float& vmax,
                         const std::string& csv_file,
                         const std::map<std::string, std::pair<float, float>>& dataset_ranges) {
    // Function to scan the directories root only, root+subdirs
    // Updated to create and initialize the CSV file with the headers 
    namespace fs = std::filesystem;
    std::string extension = (format == "hdf4") ? ".hdf" : ".h5";
    int file_count = 0;
    // bool header_written = false;

    // if (subdirs.empty() && !subsubdirs.empty()) {
    //     std::cout << "Warning: subsubdirs provided but no subdirs; they will be ignored.\n";
    // }

    // Open the mask once
    std::vector<int16_t> mask;
    if (format == "hdf4") {
        mask = read_mask_dataset_hdf4(mask_path);
    } else {
        std::cerr << "HDF5 mask handling not implemented yet.\n";
        return;
    }

    // Find the first file to get the dataset names and initialize the CSV file with headers
    fs::path first_file = find_first_valid_file(root, extension, subdirs);

    if (first_file.empty()) {
        std::cout << "No " << extension << " files found to write CSV header.\n";
        return;
    }

    std::vector<std::string> dataset_names = read_hdf4_info(first_file);

    // Write header to the CSV file
    std::ofstream out(csv_file, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open CSV file: " << csv_file << "\n";
        return;
    }

    out << "Date,";
    for (const auto& name : dataset_names) {
        for (const int16_t& mask_val : mask_values) {
            out << name << "_min_" << mask_val << ",";
            out << name << "_max_" << mask_val << ",";
            out << name << "_avg_" << mask_val << ",";
            out << name << "_stdev_" << mask_val << ",";
            out << name << "_npixels_" << mask_val << ",";
        }
    }
    out << "\n";
    out.close();

    // Lambda to process one directory
    auto process_directory = [&](const fs::path& dir_path) {
        if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) return;

        std::vector<fs::directory_entry> files;
        for (const auto& entry : fs::directory_iterator(dir_path)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                files.push_back(entry);
            }
        }

        std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
            return a.path().filename() < b.path().filename();
        });

        if (files.empty()) {
            std::cout << "No " << extension << " files found in: " << dir_path << "\n";
            return;
        }

        for (const auto& entry : files) {
            std::string filename = entry.path().filename().string();
            std::string date_str = convert_doy_date(filename);

            std::cout << "  Found file: " << filename << ", Date: " << date_str << "\n";

            ++file_count;
            if (format == "hdf5") {
                read_hdf5_info(entry.path().string()); // placeholder
            } else {
                read_hdf4_file_create_csv(entry.path().string(), mask, mask_values, vmin, vmax, csv_file, date_str, dataset_ranges);
            }
        }
    };

    // Case 1: no subdirs
    if (subdirs.empty()) {
        std::cout << "Scanning root directory: " << root << "\n";
        process_directory(root);
    } else {
        for (const auto& sub : subdirs) {
            fs::path target = fs::path(root) / sub;
            std::cout << "Scanning: " << target << "\n";
            process_directory(target);
        }
    }

    if (file_count == 0) {
        std::cout << "No " << extension << " files found in any directory.\n";
    }

    if (file_count == 0) {
        std::cout << "No " << extension << " files found in any directory.\n";
    }
}