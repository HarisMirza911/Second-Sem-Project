#include "Ems.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>

int main(int argc, char* argv[]) {
    boost::property_tree::ptree pt;
    std::string config_file;
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
            saveFile = pt.get<std::string>("SAVEFILE");
            cout << "Save file: " << saveFile << endl;
        }
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    CORE::EMS ems(saveFile);
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
                string name;
                cout << "Enter name: ";
                getline(cin, name);
                
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                
                string address;
                cout << "Enter address: ";
                getline(cin, address);
                
                int unitsUsed;
                cout << "Enter units used: ";
                cin >> unitsUsed;
                cin.ignore();

                string billingDate;
                cout << "Enter billing date(YYYY-MM-DD): ";
                getline(cin, billingDate);

                ems.addConsumer(name, id, address, unitsUsed, billingDate);
                break;
            }
            case 2: {
                ems.viewAllConsumers();
                break;
            }
            case 3: {
                cout << "Enter ID: ";
                cin >> id;
                ems.searchConsumerByID(id);
                break;
            }
            case 4: {
                cout << "Enter ID: ";
                cin >> id;
                ems.removeConsumer(id);
                break;
            }
            case 5: {
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter billing date(YYYY-MM-DD): ";
                string billingDate;
                getline(cin, billingDate);
                ems.calculateBill(id, billingDate);
                break;
            }
            case 6: {
                ems.generateRevenue();
                break;
            }
            case 7: {
                return 0;
            }
        }
    }
    return 0;
}
