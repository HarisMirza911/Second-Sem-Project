#ifndef BILLING_HPP
#define BILLING_HPP

#include <iostream>
#include <string>
#include "Consumer.hpp"

using namespace std;

namespace CORE {
    class Billing {
        Consumer consumer;
        int unitsUsed;
        friend class EMS;
        public:
            Billing() = default;
            Billing(Consumer consumer, int unitsUsed) : consumer(consumer), unitsUsed(unitsUsed) {}
            ~Billing() = default; // Changed from declaration to default implementation
        
        friend ostream& operator<<(ostream& os, const Billing& billing) {
            os << billing.consumer << "\t" << billing.unitsUsed;
            return os;
        }

        int getRate() {
            if(unitsUsed <= 100) {
                return 5;
            }
            else if(unitsUsed <= 300) {
                return 7;
            }

            return 10;
        }
    };
}

#endif