#!/usr/bin/env python3
"""Plot one experiment's output.csv: fair value + quotes, inventory, and
mark-to-market PnL over time. Saves a PNG next to the CSV and prints a short
summary to stdout.

Usage: python3 scripts/plot_experiment.py <path/to/output.csv>
"""
import sys
import os
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def main():
    if len(sys.argv) < 2:
        print("Usage: plot_experiment.py <output.csv>", file=sys.stderr)
        sys.exit(1)

    csv_path = sys.argv[1]
    df = pd.read_csv(csv_path)
    out_dir = os.path.dirname(os.path.abspath(csv_path))
    png_path = os.path.join(out_dir, "chart.png")

    fig, axes = plt.subplots(3, 1, figsize=(9, 9), sharex=True)

    axes[0].plot(df["time"], df["fair_value"], label="fair value", color="black", linewidth=1)
    axes[0].plot(df["time"], df["bid"], label="bid", color="tab:blue", linewidth=0.8, alpha=0.7)
    axes[0].plot(df["time"], df["ask"], label="ask", color="tab:red", linewidth=0.8, alpha=0.7)
    axes[0].set_ylabel("price")
    axes[0].legend(loc="upper left", fontsize=8)
    axes[0].set_title(os.path.basename(out_dir))

    axes[1].step(df["time"], df["inventory"], color="tab:purple", where="post")
    axes[1].axhline(0, color="grey", linewidth=0.5)
    axes[1].set_ylabel("inventory")

    axes[2].plot(df["time"], df["mark_to_market"], color="tab:green")
    axes[2].axhline(0, color="grey", linewidth=0.5)
    axes[2].set_ylabel("mark-to-market PnL")
    axes[2].set_xlabel("time")

    fig.tight_layout()
    fig.savefig(png_path, dpi=130)
    print(f"Saved {png_path}")

    spread = df["ask"] - df["bid"]
    print(f"rows={len(df)}")
    print(f"avg_spread={spread.mean():.4f}  min_spread={spread.min():.4f}  max_spread={spread.max():.4f}")
    print(f"inventory: min={df['inventory'].min()} max={df['inventory'].max()} final={df['inventory'].iloc[-1]}")
    print(f"final_mark_to_market={df['mark_to_market'].iloc[-1]:.4f}")
    print(f"mark_to_market: min={df['mark_to_market'].min():.4f} max={df['mark_to_market'].max():.4f} "
          f"std={df['mark_to_market'].std():.4f}")


if __name__ == "__main__":
    main()
