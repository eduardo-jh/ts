#include "stats_utils.hpp"
#include <limits>
#include <cmath>
#include <iostream>

template <typename T>
void compute_stats(const std::vector<T>& data,
                   T& min_val,
                   T& max_val,
                   float& mean,
                   float& stddev) {
    if (data.empty()) {
        std::cerr << "Vector is empty. Cannot compute statistics.\n";
        return;
    }

    min_val = std::numeric_limits<T>::max();
    max_val = std::numeric_limits<T>::lowest();
    float sum = 0.0f;
    float sum_sq = 0.0f;

    for (const T& val : data) {
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;

        float fval = static_cast<float>(val);
        sum += fval;
        sum_sq += fval * fval;
    }

    size_t n = data.size();
    mean = sum / n;
    stddev = std::sqrt((sum_sq / n) - (mean * mean));
}

// Explicit instantiations for types used
template void compute_stats<int8_t>(const std::vector<int8_t>&, int8_t&, int8_t&, float&, float&);
template void compute_stats<int16_t>(const std::vector<int16_t>&, int16_t&, int16_t&, float&, float&);
template void compute_stats<float>(const std::vector<float>&, float&, float&, float&, float&);