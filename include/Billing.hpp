#ifndef BILLING_HPP
#define BILLING_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Consumer.hpp"

namespace CORE {
    template<typename SAVETYPE> class EMS;
    
    class Billing {
        public:
            struct History {
                int unitsUsed;
                std::string billingDate;

                History(int unitsUsed, std::string billingDate) : unitsUsed(unitsUsed), billingDate(billingDate) {}
            };

            Consumer consumer;
            std::vector<History> history;

            Billing() = default;
            Billing(Consumer consumer, std::vector<History> history) : consumer(consumer), history(history) {}
            Billing(Consumer consumer, History history) : consumer(consumer) {
                this->history.push_back(history);
            }
            ~Billing() = default;
        
            friend std::ostream& operator<<(std::ostream& os, const Billing& billing) {
                os << billing.consumer << "\t";
                for(const auto& history : billing.history) {
                    os << history.unitsUsed << " for " << history.billingDate << "\t";
                }
                return os;
            }

            std::pair<int, int> getRate(std::string billingDate) {
                auto currentEntry = std::find_if(history.begin(), history.end(),
                    [&billingDate](const History& h) { return h.billingDate == billingDate; });
                
                if (currentEntry == history.end()) {
                    return std::make_pair(0, 0);
                }
                
                int unitsUsed = currentEntry->unitsUsed;
                
                if (currentEntry != history.begin()) {
                    auto prevEntry = std::prev(currentEntry);
                    unitsUsed = currentEntry->unitsUsed - prevEntry->unitsUsed;
                }
                
                if (unitsUsed <= 100) {
                    return std::make_pair(5, unitsUsed);
                }
                else if (unitsUsed <= 300) {
                    return std::make_pair(7, unitsUsed);
                }
                
                return std::make_pair(10, unitsUsed);
            }

            std::pair<int, int> getRate() {
                if (history.empty()) {
                    return std::make_pair(0, 0);
                }
                
                auto currentEntry = history.back();
                
                int unitsUsed = currentEntry.unitsUsed;
                
                if (history.size() > 1) {
                    auto prevEntry = history[history.size() - 2];
                    unitsUsed = currentEntry.unitsUsed - prevEntry.unitsUsed;
                }
                
                if (unitsUsed <= 100) {
                    return std::make_pair(5, unitsUsed);
                }
                else if (unitsUsed <= 300) {
                    return std::make_pair(7, unitsUsed);
                }
                
                return std::make_pair(10, unitsUsed);
            }
            
            template<typename T> friend class EMS;
            friend class DataHandling::FileHandler;
            friend class DataHandling::Database;
    };
}

#endif