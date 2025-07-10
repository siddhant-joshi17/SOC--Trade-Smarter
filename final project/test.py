from data_processor import load_data, add_bollinger_bands
from model import load_model

df = load_data("data/BDL_testing.csv")
df = add_bollinger_bands(df)

features = ['macd', 'macd_signal', 'rsi', 'bollinger_signal']
df = df.dropna()

model = load_model("models/nn_model_weights.h5")
df['pred'] = model.predict(df[features])
df['return'] = df['Close'].pct_change().shift(-1)

strategy = df[df['pred'] == 1]
success_rate = (strategy['return'] > 0).mean() * 100
avg_return = strategy['return'].mean()
trades = len(strategy)

print("📊 Neural Network Strategy Results")
print(f"✅ Success Rate: {success_rate:.2f}%")
print(f"📈 Per-Trade Return: {avg_return:.2f}")
print(f"🔁 Total Trades Executed: {trades}")
