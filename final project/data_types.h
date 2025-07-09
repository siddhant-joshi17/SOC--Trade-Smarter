#pragma once
#include <vector>
#include <string>

struct MarketData {
    std::vector<std::string> dates;
    std::vector<double> open;
    std::vector<double> high;
    std::vector<double> low;
    std::vector<double> close;
    std::vector<double> volume;
};
