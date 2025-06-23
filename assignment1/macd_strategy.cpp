#include "macd_strategy.h"
#include <vector>
#include <iostream>

using namespace std;

// EMA Calculation
double calculate_ema(const vector<double>& prices, int period, int current_index, double prev_ema = -1) {
    double k = 2.0 / (period + 1);
    if (prev_ema < 0) {
        double sma = 0.0;
        for (int i = current_index - period + 1; i <= current_index; ++i)
            sma += prices[i];
        return sma / period;
    } else {
        return prices[current_index] * k + prev_ema * (1 - k);
    }
}

// MACD Strategy Runner
TradeResult run_macd_strategy(const vector<Candle>& candles, double profit_threshold) {
    vector<double> closes;
    for (const auto& candle : candles)
        closes.push_back(candle.close);

    size_t n = closes.size();
    vector<double> macd_line(n, 0.0), signal_line(n, 0.0), histogram(n, 0.0);
    vector<int> macd_signals(n, 0);

    double ema12 = -1, ema26 = -1, ema_signal = -1;
    vector<double> macd_values;

    int total_trades = 0;
    int profitable_trades = 0;
    double total_return = 0.0;

    double entry_price = 0.0;
    int entry_index = -1;
    enum Position { NONE, LONG, SHORT } state = NONE;

    for (size_t i = 26; i < n; ++i) {
        ema12 = calculate_ema(closes, 12, i, ema12);
        ema26 = calculate_ema(closes, 26, i, ema26);
        macd_line[i] = ema12 - ema26;

        macd_values.push_back(macd_line[i]);
        if (macd_values.size() >= 9) {
            ema_signal = calculate_ema(macd_values, 9, macd_values.size() - 1, ema_signal);
            signal_line[i] = ema_signal;
            histogram[i] = macd_line[i] - signal_line[i];
        }

        // Generate signals
        if (i > 26 && histogram[i - 1] < 0 && histogram[i] > 0) {
            macd_signals[i] = 1;  // Bullish crossover - Buy
        } else if (i > 26 && histogram[i - 1] > 0 && histogram[i] < 0) {
            macd_signals[i] = -1; // Bearish crossover - Sell
        }

        // Trading logic
        if (state == NONE) {
            if (macd_signals[i] == 1) {
                state = LONG;
                entry_price = closes[i];
                entry_index = i;
            } else if (macd_signals[i] == -1) {
                state = SHORT;
                entry_price = closes[i];
                entry_index = i;
            }
        } else if (state == LONG && macd_signals[i] == -1) {
            double ret = (closes[i] - entry_price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold) profitable_trades++;
            total_trades++;
            state = NONE;
        } else if (state == SHORT && macd_signals[i] == 1) {
            double ret = (entry_price - closes[i]) / entry_price;
            total_return += ret;
            if (ret > profit_threshold) profitable_trades++;
            total_trades++;
            state = NONE;
        }
    }

    // Force close
    if (state != NONE) {
        double final_price = closes.back();
        double ret = 0.0;
        if (state == LONG)
            ret = (final_price - entry_price) / entry_price;
        else if (state == SHORT)
            ret = (entry_price - final_price) / entry_price;

        total_return += ret;
        if (ret > profit_threshold) profitable_trades++;
        total_trades++;
    }

    double success_rate = total_trades > 0 ? (double)profitable_trades / total_trades * 100 : 0;
    double avg_return = total_trades > 0 ? (total_return / total_trades) * 100 : 0;

    return {success_rate, avg_return, total_trades, macd_signals};
}

