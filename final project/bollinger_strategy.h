#pragma once
#include <vector>

std::vector<int> bollinger_signal(const std::vector<double>& close, const std::vector<double>& lower_band);
