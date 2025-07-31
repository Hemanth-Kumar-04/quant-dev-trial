# quant-dev-trial



Overview

This repository implements a single-pass C++ solution to reconstruct the Market-by-Price top 10 levels (MBP-10) order book from a Market-by-Order (MBO) CSV input. It faithfully follows the Blockhouse Quantitative Developer trial requirements, including special handling of the T→F→C trade/cancel sequences and no-side ('N') trades.





#Run

GO TO THE ROOT OF REPO: .\reconstruction_hemanth.exe data\mbo.csv > out.csv



#BUILD BINARY FILE IF NEEDED: make



# Clone and build

git clone https://github.com/<your-username>/quant-dev-trial.git

cd quant-dev-trial

make  

#RUN

.\reconstruction_hemanth.exe data\mbo.csv > out.csv





#Performance & Optimizations



Single-pass file streaming: O(#rows × log #levels).



O(1) order lookup + O(log n) level updates via unordered_map + std::map.



Lazy string formatting: two-decimal conversion only at output time.



Minimal memory footprint: stores only active orders and snapshots.





#Key Components



main.cpp: Entry point that



Reads mbo.csv line-by-line using CSVParser.



Tracks pending T→F→C sequences: defers applying book changes until the final C, corrects side to actual liquidity removal side, and collapses into one T event in the MBP output.



Maintains an OrderBook instance for active orders, resets on R actions.



After each actionable MBO row, retrieves top 10 bids and asks, formats them to two decimals, and stores them plus original fields for final emission.



OrderBook (OrderBook.h / OrderBook.cpp):



Stores pending orders in an unordered_map<OrderID, Order>.



Maintains sorted price levels in std::map<double, Level>: bids (descending) and asks (ascending).



addOrder(), tradeCancel() methods update levels and order sizes/counts in O(log n).



topLevels(side, depth) returns the top N levels for snapshotting.



CSVParser (utils/CSVParser.h / CSVParser.cpp):



Splits each line by , into a std::vector<std::string> for simple, dependency-free parsing.





Requires a C++17-compatible compiler (e.g., g++ ≥ 7.0)

