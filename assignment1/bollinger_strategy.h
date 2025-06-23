#pragma once
#include <vector>

struct Candle {
    double open;
    double high;
    double low;
    double close;
};

struct TradeResult {
    double success_rate;
    double average_return;
    int total_trades;
    std::vector<int> bb_signals; // 1 = Buy, -1 = Sell, 0 = No Action
};
