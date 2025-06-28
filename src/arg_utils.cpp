#include <iostream>
#include <sstream>
#include "arg_utils.hpp"

void print_help() {
    std::cout << "Usage: ts [OPTIONS]\n"
              << "Options:\n"
              << "  -d, --dir PATH              Specify directory to proceess files\n"
              << "  -f, --format FORMAT         Specify HDF format: hdf4 or hdf5 (default: hdf4)\n"
              << "  -s, --subdirs SUBDIRS       Comma-separated list of subdirectories under --dir\n"
              << "                              scans each <dir>/<subdir> (e.g. -s 2011,2012)\n"
              << "  -m, --mask FILE             Specify a mask file (only HDF4 currently supported)\n"
              << "  -V, --values VALUES         Comma-separated list of mask values (default: 1) to extract from datasets\n"
              << "  -l, --lower VALUE           Exclude values less than VALUE from analysis and stats (default: -9999)\n"
              << "  -u, --upper VALUE           Exclude values greater than VALUE from analysis and stats (default: 20000)\n"
              << "  -r, --ranges VALUE          Specify the valid ranges for each dataset, otherwise use values from -u -l\n"
              << "                              format is DS:VMIN:VMAX comma separated (e.g. NDVI:-2000:10000,ET:0.1:20)\n"
              << "  -o, --output FILE           Output CSV file to save the time series statistics\n"
              << "  -v, --version               Show version information\n"
              << "  -h, --help                  Show this help message\n";
}

void print_version() {
    std::cout << "ts - time series utilities for HDF4 and HDF5 data, version 1.0\n"
              << "\nCopyright (C) 2025 Eduardo Jimenez Hernandez <eduardojh@arizona.edu>.\n"
              << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
              << "This is free software: you are free to change and redistribute it.\n"
              << "There is NO WARRANTY, to the extent permitted by law.\n";
}

std::vector<std::string> split_list(const std::string& str) {
    // Splits a list of comma separated strings into a vector
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) result.push_back(token);
    }
    return result;
}

std::vector<int16_t> split_list_int(const std::string& str) {
    // Splits a list of comma separated strings into a vector of int16
    std::vector<int16_t> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) result.push_back(std::stoi(token));
    }
    return result;
}
