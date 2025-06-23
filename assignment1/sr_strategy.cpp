#include "sr_strategy.h"
#include <vector>
#include <algorithm>

using namespace std;

// Detect support level (lowest low in recent period)
double find_support(const vector<Candle>& candles, int index, int lookback = 10) {
    double support = candles[index].low;
    for (int i = max(0, index - lookback + 1); i <= index; ++i)
        support = min(support, candles[i].low);
    return support;
}

// Detect resistance level (highest high in recent period)
double find_resistance(const vector<Candle>& candles, int index, int lookback = 10) {
    double resistance = candles[index].high;
    for (int i = max(0, index - lookback + 1); i <= index; ++i)
        resistance = max(resistance, candles[i].high);
    return resistance;
}

// Run S&R strategy
TradeResult run_sr_strategy(const vector<Candle>& candles, double profit_threshold, int lookback = 10) {
    int n = candles.size();
    vector<int> sr_signals(n, 0);

    enum Position { NONE, LONG, SHORT } state = NONE;
    double entry_price = 0.0;
    int total_trades = 0, profitable_trades = 0;
    double total_return = 0.0;

    for (int i = lookback; i < n; ++i) {
        double support = find_support(candles, i - 1, lookback);
        double resistance = find_resistance(candles, i - 1, lookback);
        double price = candles[i].close;

        // Signal generation
        if (price <= support * 1.01) {
            sr_signals[i] = 1; // Near support: Buy
        } else if (price >= resistance * 0.99) {
            sr_signals[i] = -1; // Near resistance: Sell
        }

        // Trading logic
        if (state == NONE) {
            if (sr_signals[i] == 1) {
                state = LONG;
                entry_price = price;
            } else if (sr_signals[i] == -1) {
                state = SHORT;
                entry_price = price;
            }
        } else if (state == LONG && price >= resistance) {
            double ret = (price - entry_price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold) ++profitable_trades;
            ++total_trades;
            state = NONE;
        } else if (state == SHORT && price <= support) {
            double ret = (entry_price - price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold) ++profitable_trades;
            ++total_trades;
            state = NONE;
        }
    }

    // Force close
    if (state != NONE) {
        double final_price = candles.back().close;
        double ret = (state == LONG)
            ? (final_price - entry_price) / entry_price
            : (entry_price - final_price) / entry_price;

        total_return += ret;
        if (ret > profit_threshold) ++profitable_trades;
        ++total_trades;
    }

    double success_rate = total_trades > 0 ? (double)profitable_trades / total_trades * 100 : 0;
    double avg_return = total_trades > 0 ? (total_return / total_trades) * 100 : 0;

    return { success_rate, avg_return, total_trades, sr_signals };
}
