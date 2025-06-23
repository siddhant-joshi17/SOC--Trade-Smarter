#include "bollinger_strategy.h"
#include <vector>
#include <cmath>

using namespace std;

// Compute SMA
double calculate_sma(const vector<double>& prices, int current_index, int period) {
    double sum = 0.0;
    for (int i = current_index - period + 1; i <= current_index; ++i)
        sum += prices[i];
    return sum / period;
}

// Compute standard deviation
double calculate_stddev(const vector<double>& prices, int current_index, int period, double sma) {
    double variance = 0.0;
    for (int i = current_index - period + 1; i <= current_index; ++i)
        variance += pow(prices[i] - sma, 2);
    return sqrt(variance / period);
}

// Run Bollinger Band strategy
TradeResult run_bollinger_strategy(const vector<Candle>& candles, double profit_threshold, int period = 20, double k = 2.0) {
    vector<double> closes;
    for (const auto& candle : candles)
        closes.push_back(candle.close);

    int n = closes.size();
    vector<int> bb_signals(n, 0);
    int total_trades = 0, profitable_trades = 0;
    double total_return = 0.0;

    enum Position { NONE, LONG, SHORT } state = NONE;
    double entry_price = 0.0;

    for (int i = period; i < n; ++i) {
        double sma = calculate_sma(closes, i, period);
        double stddev = calculate_stddev(closes, i, period, sma);
        double upper = sma + k * stddev;
        double lower = sma - k * stddev;
        double price = closes[i];

        // Signal generation
        if (price < lower) {
            bb_signals[i] = 1; // Buy signal
        } else if (price > upper) {
            bb_signals[i] = -1; // Sell signal
        } else {
            bb_signals[i] = 0;
        }

        // Strategy logic
        if (state == NONE) {
            if (price < lower) {
                state = LONG;
                entry_price = price;
            } else if (price > upper) {
                state = SHORT;
                entry_price = price;
            }
        } else if (state == LONG && price >= sma) {
            double ret = (price - entry_price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold) ++profitable_trades;
            ++total_trades;
            state = NONE;
        } else if (state == SHORT && price <= sma) {
            double ret = (entry_price - price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold) ++profitable_trades;
            ++total_trades;
            state = NONE;
        }
    }

    // Force close
    if (state != NONE) {
        double final_price = closes.back();
        double ret = (state == LONG)
            ? (final_price - entry_price) / entry_price
            : (entry_price - final_price) / entry_price;

        total_return += ret;
        if (ret > profit_threshold) ++profitable_trades;
        ++total_trades;
    }

    double success_rate = total_trades > 0 ? (double)profitable_trades / total_trades * 100 : 0;
    double avg_return = total_trades > 0 ? (total_return / total_trades) * 100 : 0;

    return { success_rate, avg_return, total_trades, bb_signals };
}

