#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <cstdint>
#include "utils/CSVParser.h"
#include "OrderBook.h"

std::string format2dec(double val) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << val;
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " mbo.csv\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }

    std::string line;
    OrderBook ob;

    bool useBid[10] = {false}, useAsk[10] = {false};
    std::vector<std::vector<std::string>> allRows;

    std::getline(in, line);  

    // State tracking for T/F/C sequence
    struct PendingTrade {
        OrderID oid;
        double price;
        int size;
        char original_side;
        bool pending;
        std::vector<std::string> original_row; // Store the original row data
    } pendingTrade = {0, 0.0, 0, 'N', false, {}};

    while (std::getline(in, line)) {
        CSVParser p(line);
        const auto& f = p.fields();
        std::vector<std::string> row;
        for (int i = 0; i <= 12; ++i) {
            if (i == 7) {  
                if (!f[7].empty()) {
                    try {
                        double raw = std::stod(f[7]);
                        row.push_back(format2dec(raw));    
                    } catch (...) {
                        row.push_back("");                
                    }
                } else {
                    row.push_back("");                      
                }
            }
            else {
                row.push_back(f[i]);
            }
        }

        char action = f[5][0];
        char side   = f[6][0];
        OrderID oid = std::stoll(f[11]);
        double price = f[7].empty()  
                       ? std::numeric_limits<double>::quiet_NaN()
                       : std::stod(f[7]);
        int sz       = std::stoi(f[9]);
        std::string symbol = (f.size() > 15 ? f[15] : "");

        // Handle special T/F/C logic according to task requirements
        bool processOrderbook = true;
        
        if (action == 'R') {
            ob.reset();
            pendingTrade.pending = false;
        } else if (action == 'A') {
            ob.addOrder({oid, price, sz, side});
        } else if (action == 'T' && side != 'N') {
            // Start of T/F/C sequence - store the trade info but don't apply yet
            pendingTrade = {oid, price, sz, side, true, row};
            processOrderbook = false; // Don't process orderbook state yet
        } else if (action == 'F' && pendingTrade.pending && oid == pendingTrade.oid) {
            // Fill action - part of T/F/C sequence, don't process orderbook
            processOrderbook = false;
        } else if (action == 'C' && pendingTrade.pending && oid == pendingTrade.oid) {
            // Cancel action - this is where we actually modify the orderbook
            // According to task: T action appears on opposite side from where order actually exists
            // So if T appeared on ASK side, the actual order is on BID side
            char actual_side = (pendingTrade.original_side == 'A') ? 'B' : 'A';
            
            // Update the stored row to reflect the corrected information
            row = pendingTrade.original_row; // Use the original T action row
            row[5] = "T"; // Ensure action is T (combining T/F/C into single T)
            row[6] = std::string(1, actual_side); // Correct the side
            
            // Apply the trade/cancel to orderbook
            ob.tradeCancel(oid, sz);
            pendingTrade.pending = false;
        } else if (action == 'C' && !pendingTrade.pending) {
            // Regular cancel not part of T/F/C sequence
            ob.tradeCancel(oid, sz);
        } else if ((action == 'T' || action == 'F' || action == 'C') && side == 'N') {
            // Side is 'N' - don't alter orderbook as per requirement 3
            processOrderbook = false;
        }

        // Only process orderbook state if we should
        if (processOrderbook) {
            auto bids = ob.topLevels('B');
            auto asks = ob.topLevels('A');

            std::vector<std::string> mbpFields;

            for (int i = 0; i < 10; ++i) {
                if (i < (int)bids.size()) {
                    useBid[i] = true;

                    std::ostringstream px;
                    px << std::fixed << std::setprecision(2) << bids[i].first;

                    mbpFields.push_back(px.str());
                    mbpFields.push_back(std::to_string(bids[i].second.size));
                    mbpFields.push_back(std::to_string(bids[i].second.count));
                } else {
                    mbpFields.insert(mbpFields.end(), {"", "0", "0"});
                }
            }

            for (int i = 0; i < 10; ++i) {
                if (i < (int)asks.size()) {
                    useAsk[i] = true;

                    std::ostringstream px;
                    px << std::fixed << std::setprecision(2) << asks[i].first;

                    mbpFields.push_back(px.str());
                    mbpFields.push_back(std::to_string(asks[i].second.size));
                    mbpFields.push_back(std::to_string(asks[i].second.count));
                } else {
                    mbpFields.insert(mbpFields.end(), {"", "0", "0"});
                }
            }

            row.insert(row.end(), mbpFields.begin(), mbpFields.end());
            row.push_back(symbol);
            row.push_back(std::to_string(oid));
            allRows.push_back(row);
        }
    }

    std::cout << ",";  
    std::cout << "ts_recv,ts_event,rtype,publisher_id,instrument_id,action,side,depth,price,size,flags,ts_in_delta,sequence";

    for (int i = 0; i < 10; ++i) {
        if (useBid[i]) {
            std::cout
              << ",bid_px_" << std::setw(2) << std::setfill('0') << i
              << ",bid_sz_" << std::setw(2) << i
              << ",bid_ct_" << std::setw(2) << i;
        }
        if (useAsk[i]) {
            std::cout
              << ",ask_px_" << std::setw(2) << std::setfill('0') << i
              << ",ask_sz_" << std::setw(2) << i
              << ",ask_ct_" << std::setw(2) << i;
        }
    }
    std::cout << ",symbol,order_id\n";

    int sno = 1;
    for (const auto& row : allRows) {
        std::cout << sno++;
        for (int col = 0; col < (int)row.size(); ++col) {
            if (col <= 12) {
                std::cout << "," << row[col];
                continue;
            }

            if (col >= 13 && col < 43) {
                int level = (col - 13) / 3;
                if (!useBid[level]) continue;

                if ((col - 13) % 3 == 0) {
                    if (!row[col].empty()) {
                        try {
                            std::cout << "," << format2dec(std::stod(row[col]));
                        } catch (...) {
                            std::cout << ",";
                        }
                    } else {
                        std::cout << ",";
                    }
                } else {
                    if (!row[col].empty()) {
                        try {
                            std::cout << "," << std::stoi(row[col]);
                        } catch (...) {
                            std::cout << ",0";
                        }
                    } else {
                        std::cout << ",0";
                    }
                }
                continue;
            }

            if (col >= 43 && col < 73) {
                int level = (col - 43) / 3;
                if (!useAsk[level]) continue;

                if ((col - 43) % 3 == 0) {
                    if (!row[col].empty()) {
                        try {
                            std::cout << "," << format2dec(std::stod(row[col]));
                        } catch (...) {
                            std::cout << ",";
                        }
                    } else {
                        std::cout << ",";
                    }
                } else {
                    if (!row[col].empty()) {
                        try {
                            std::cout << "," << std::stoi(row[col]);
                        } catch (...) {
                            std::cout << ",0";
                        }
                    } else {
                        std::cout << ",0";
                    }
                }
                continue;
            }

            std::cout << "," << row[col];
        }
        std::cout << "\n";
    }

    return 0;
}