import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("data.csv")

fig, ax = plt.subplots(1, 1)
ax.plot(df['n_proc'], df['time'])
ax.set_xlabel("Number of processes")
ax.set_ylabel("Processing time (s)")
plt.show()