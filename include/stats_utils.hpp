#pragma once

#include <cstdint>
#include <vector>

// Declaration only
template <typename T>
void compute_stats(const std::vector<T>& data,
                   T& min_val,
                   T& max_val,
                   float& mean,
                   float& stddev);

// Explicit instantiations (optional if defined elsewhere)
extern template void compute_stats<int8_t>(const std::vector<int8_t>&, int8_t&, int8_t&, float&, float&);
extern template void compute_stats<int16_t>(const std::vector<int16_t>&, int16_t&, int16_t&, float&, float&);
extern template void compute_stats<float>(const std::vector<float>&, float&, float&, float&, float&);
