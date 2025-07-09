#include "bollinger_strategy.h"

std::vector<int> bollinger_signal(const std::vector<double>& close, const std::vector<double>& lower_band) {
    std::vector<int> signal(close.size(), 0);
    for (size_t i = 0; i < close.size(); ++i) {
        if (close[i] < lower_band[i]) {
            signal[i] = 1; // Buy
        }
    }
    return signal;
}
