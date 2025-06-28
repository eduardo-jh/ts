#include <cstdint>
#include <string>
#include <iostream>
#include <getopt.h>
#include <sstream>
#include <vector>
#include <map>

#include "arg_utils.hpp"
#include "file_utils.hpp"

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