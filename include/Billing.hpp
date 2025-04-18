#ifndef BILLING_HPP
#define BILLING_HPP

#include <iostream>
#include <string>
#include "Consumer.hpp"


using namespace std;

namespace CORE {
    class Billing {
        
        struct History {
            int unitsUsed;
            string billingDate;

            History(int unitsUsed, string billingDate) : unitsUsed(unitsUsed), billingDate(billingDate) {}
        };

        Consumer consumer;
        vector<History> history;

        friend class EMS;
        public:
            Billing() = default;
            Billing(Consumer consumer, vector<History> history) : consumer(consumer), history(history) {}
            Billing(Consumer consumer, History history) : consumer(consumer) {
                this->history.push_back(history);
            }
            ~Billing() = default; // Changed from declaration to default implementation
        
        friend ostream& operator<<(ostream& os, const Billing& billing) {
            os << billing.consumer << "\t";
            for(const auto& history : billing.history) {
                os << history.unitsUsed << " for " << history.billingDate << "\t";
            }
            return os;
        }

        std::pair<int, int> getRate(string billingDate) {
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
    };
}

#endif