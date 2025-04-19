#include "Ems.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <atomic>
#include <csignal>

std::unique_ptr<CORE::EMS<DataHandling::FileHandler>> fileEms;
std::unique_ptr<CORE::EMS<DataHandling::Database>> dbEms;

void signalHandler(int signum) {
    std::cout << "Interrupt Signal Received" << std::endl;
    std::cout << "Stopping the program" << std::endl;

    if(fileEms) {
        fileEms.reset();
    }

    if(dbEms) {
        dbEms.reset();
    }
    exit(signum);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGBUS, signalHandler);
    boost::property_tree::ptree pt;
    std::string config_file;
    std::string saveType;
    std::string saveFile;
    try {
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("config,c", boost::program_options::value<std::string>(&config_file)->required(), "set config file");
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
        if(vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }
        if(vm.count("config")) {
            boost::property_tree::ini_parser::read_ini(config_file, pt);
            saveType = pt.get<std::string>("SAVETYPE");
            saveFile = pt.get<std::string>("SAVEFILE");
            std::cout << "Save type: " << saveType << std::endl;
            std::cout << "Save file: " << saveFile << std::endl;
        }
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Create a pointer to hold our EMS instance
    if(saveType == "file") {
        fileEms = std::make_unique<CORE::EMS<DataHandling::FileHandler>>(
            saveFile, 
            std::move(DataHandling::FileHandler(saveFile))
        );
    } else if(saveType == "database") {
        std::string host = std::getenv("MYSQL_HOST");
        if(host.empty()) {
            std::cerr << "MYSQL_HOST is not set" << std::endl;
            return 1;
        }
        std::string user = std::getenv("MYSQL_USER");
        if(user.empty()) {
            std::cerr << "MYSQL_USER is not set" << std::endl;
            return 1;
        }
        std::string password = std::getenv("MYSQL_PASSWORD");
        if(password.empty()) {
            std::cerr << "MYSQL_PASSWORD is not set" << std::endl;
            return 1;
        }
        std::string database = std::getenv("MYSQL_DATABASE");
        if(database.empty()) {
            std::cerr << "MYSQL_DATABASE is not set" << std::endl;
            return 1;
        }
        dbEms = std::make_unique<CORE::EMS<DataHandling::Database>>(
            saveFile, 
            std::move(DataHandling::Database(host, user, password, database))
        );
    }
    
    // Define a macro to call the appropriate EMS instance
    #define CALL_EMS(method, ...) \
        if(fileEms) fileEms->method(__VA_ARGS__); \
        else if(dbEms) dbEms->method(__VA_ARGS__);
    
    while(true) {
        std::cout << "1. Add Consumer" << std::endl;
        std::cout << "2. View All Consumers" << std::endl;
        std::cout << "3. Search Consumer by ID" << std::endl;
        std::cout << "4. Remove Consumer" << std::endl;
        std::cout << "5. Calculate Bill" << std::endl;
        std::cout << "6. Generate Revenue" << std::endl;
        std::cout << "7. Exit" << std::endl;
        int choice;
        std::cin >> choice;
        
        // Clear input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        // Variables used in multiple cases
        int id;
        
        switch(choice) {
            case 1: {
                std::string name;
                std::cout << "Enter name: ";
                std::getline(std::cin, name);
                
                std::cout << "Enter ID: ";
                std::cin >> id;
                std::cin.ignore();
                
                std::string address;
                std::cout << "Enter address: ";
                std::getline(std::cin, address);
                
                int unitsUsed;
                std::cout << "Enter units used: ";
                std::cin >> unitsUsed;
                std::cin.ignore();

                std::string billingDate;
                std::cout << "Enter billing date(YYYY-MM-DD): ";
                std::getline(std::cin, billingDate);

                CALL_EMS(addConsumer, name, id, address, unitsUsed, billingDate);
                break;
            }
            case 2: {
                if(fileEms) fileEms->viewAllConsumers();
                else if(dbEms) dbEms->viewAllConsumers();
                break;
            }
            case 3: {
                std::cout << "Enter ID: ";
                std::cin >> id;
                CALL_EMS(searchConsumerByID, id);
                break;
            }
            case 4: {
                std::cout << "Enter ID: ";
                std::cin >> id;
                CALL_EMS(removeConsumer, id);
                break;
            }
            case 5: {
                std::cout << "Enter ID: ";
                std::cin >> id;
                std::cin.ignore();
                std::cout << "Enter billing date(YYYY-MM-DD): ";
                std::string billingDate;
                std::getline(std::cin, billingDate);
                CALL_EMS(calculateBill, id, billingDate);
                break;
            }
            case 6: {
                if(fileEms) fileEms->generateRevenue();
                else if(dbEms) dbEms->generateRevenue();
                break;
            }
            case 7: {
                return 0;
            }
        }
    }
    return 0;
}
