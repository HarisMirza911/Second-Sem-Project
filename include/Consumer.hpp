#ifndef CONSUMER_HPP
#define CONSUMER_HPP

#include <iostream>
#include <string>

// Forward declaration of DataHandling namespace
namespace DataHandling {
    class FileHandler;
    class Database;
}

namespace CORE {
    // Forward declaration of EMS template class
    template<typename SAVETYPE> class EMS;
    
    class Consumer {
        public: // Make these public
            std::string name;
            int id;
            std::string address;
            
        public:
            Consumer() = default;
            Consumer(std::string name, int id, std::string address) : name(name), id(id), address(address) {}
            ~Consumer() = default;

            friend std::ostream& operator<<(std::ostream& os, const Consumer& consumer) {
                os << consumer.name << "\t" << consumer.id << "\t" << consumer.address;
                return os;
            }
            
            // Make these classes friends so they can access private members
            template<typename T> friend class EMS;
            friend class DataHandling::FileHandler;
            friend class DataHandling::Database;
    };
}

#endif