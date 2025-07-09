#include "macd_strategy.h"

std::vector<int> macd_signal(const std::vector<double>& macd, const std::vector<double>& signal) {
    std::vector<int> result(macd.size(), 0);
    for (size_t i = 1; i < macd.size(); ++i) {
        if (macd[i] > signal[i] && macd[i - 1] <= signal[i - 1])
            result[i] = 1;
    }
    return result;
}
