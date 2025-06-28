
#include "hdf5_utils.hpp"

void read_hdf5_info(const std::string& file_path) {
    hid_t file_id = H5Fopen(file_path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) {
        std::cerr << "Failed to open HDF5 file: " << file_path << "\n";
        return;
    }

    std::cout << "Opened HDF5 file successfully: " << file_path << "\n";
    H5Fclose(file_id);
}
