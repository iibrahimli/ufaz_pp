import pandas as pd
import matplotlib.pyplot as plt

local_df = pd.read_csv("local.csv")
remote_df = pd.read_csv("remote.csv")

fig, (ax1, ax2) = plt.subplots(1, 2)

ax1.set_title("Total time")
ax1.set_xlabel("Message length (bytes)")
ax1.set_ylabel("Total time (s)")
ax1.set_xscale('log')
ax1.set_yscale('log')
ax1.plot(local_df['message_len'], local_df['total_time'], label='Local')
ax1.plot(local_df['message_len'], remote_df['total_time'], label='Remote')
ax1.legend()


ax2.set_title("Throughput")
ax2.set_xlabel("Message length (bytes)")
ax2.set_ylabel("Throughput (MiB/s)")
ax2.set_xscale('log')
ax2.set_yscale('log')
ax2.plot(local_df['message_len'], local_df['throughput'], label='Local')
ax2.plot(local_df['message_len'], remote_df['throughput'], label='Remote')
ax2.legend()

plt.show()