#include "rsi_strategy.h"

std::vector<int> rsi_signal(const std::vector<double>& rsi) {
    std::vector<int> signal(rsi.size(), 0);
    for (size_t i = 0; i < rsi.size(); ++i) {
        if (rsi[i] < 30)
            signal[i] = 1;
    }
    return signal;
}
