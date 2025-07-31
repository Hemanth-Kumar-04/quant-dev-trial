# quant-dev-trial

## Overview

This repository implements a single-pass C++ solution to reconstruct the Market-by-Price top 10 levels (MBP-10) order book from a Market-by-Order (MBO) CSV input. It faithfully follows the Blockhouse Quantitative Developer trial requirements, including special handling of the T→F→C trade/cancel sequences and no-side ('N') trades.

## Run

Make sure you are in the root of the repository, then run:

```bash
./reconstruction_hemanth.exe data/mbo.csv > out.csv
```

## Build

If the binary is not built yet, you can compile it using:

```bash
make
```

## Clone and Build

```bash
git clone https://github.com/Hemanth-Kumar-04/quant-dev-trial.git
cd quant-dev-trial
make
```

## Usage

```bash
./reconstruction_hemanth.exe data/mbo.csv > out.csv
```

- **Input**: `mbo.csv` – the raw market-by-order stream
- **Output**: CSV output matching MBP-10 snapshot format

## Performance & Optimizations

- Single-pass file streaming: O(#rows × log #levels)
- O(1) order lookup + O(log n) level updates via `unordered_map` + `std::map`
- Lazy string formatting: two-decimal conversion only at output time
- Minimal memory footprint: stores only active orders and snapshots

## Key Components

### `main.cpp`

- Reads `mbo.csv` line-by-line using `CSVParser`
- Tracks pending T→F→C sequences: defers applying book changes until the final `C`, corrects side to actual liquidity removal side, and collapses into one `T` event in the MBP output
- Maintains an `OrderBook` instance for active orders, resets on `R` actions
- After each actionable MBO row, retrieves top 10 bids and asks, formats them to two decimals, and stores them along with original fields

### `OrderBook.h` / `OrderBook.cpp`

- Stores pending orders in an `unordered_map<OrderID, Order>`
- Maintains sorted price levels in `std::map<double, Level>`: `bids` (descending) and `asks` (ascending)
- `addOrder()`, `tradeCancel()` methods update levels and order sizes/counts in O(log n)
- `topLevels(side, depth)` returns the top N levels for snapshotting

### `utils/CSVParser.h` / `CSVParser.cpp`

- Splits each line by `,` into a `std::vector<std::string>` for simple, dependency-free parsing

## Requirements

- Requires a C++17-compatible compiler (e.g., `g++` ≥ 7.0)
- GNU Make

---

*Thank you for reviewing this implementation!*

