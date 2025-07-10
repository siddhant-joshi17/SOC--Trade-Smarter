import pandas as pd

def load_data(path):
    df = pd.read_csv(path, parse_dates=['Date'], index_col='Date')
    return df

def add_bollinger_bands(df, window=20, num_std=2):
    df['ma'] = df['Close'].rolling(window).mean()
    df['std'] = df['Close'].rolling(window).std()
    df['upper_band'] = df['ma'] + (num_std * df['std'])
    df['lower_band'] = df['ma'] - (num_std * df['std'])
    df['bollinger_signal'] = (df['Close'] < df['lower_band']).astype(int)
    return df
