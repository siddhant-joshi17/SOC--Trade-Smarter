from data_processor import load_data, add_bollinger_bands

df = load_data("data/BDL_training.csv")
df = add_bollinger_bands(df)

features = ['macd', 'macd_signal', 'rsi', 'bollinger_signal']
df = df.dropna()

X = df[features]
y = (df['Close'].shift(-1) > df['Close']).astype(int)

from model import build_model, save_model

model = build_model()
model.fit(X, y)
save_model(model, "models/nn_model_weights.h5")
