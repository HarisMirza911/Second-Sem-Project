#ifndef CONSUMER_HPP
#define CONSUMER_HPP

#include <iostream>
#include <string>
using namespace std;

namespace CORE {
    class EMS;
    class Consumer {
            string name;
            int id;
            string address;
            friend class EMS;
        public:
            Consumer() = default;
            Consumer(string name, int id, string address) : name(name), id(id), address(address) {}
            ~Consumer() = default; // Changed from declaration to default implementation

        friend ostream& operator<<(ostream& os, const Consumer& consumer) {
            os << consumer.name << "\t" << consumer.id << "\t" << consumer.address;
            return os;
        }
    };
}

#endif