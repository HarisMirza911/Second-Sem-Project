#ifndef EMS_HPP
#define EMS_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <rapidjson/document.h>
#include "Billing.hpp"


namespace CORE {
    class EMS {
        static int count;
        std::unordered_map<int, Billing> consumers;
        bool revenueGenerated = false;
        int revenue = 0;
        string saveFile;
        // {"name":"Haris", "id":1, "address":"123 Main St", "data":[{"Billing Date":"2025-01-01", "Units Used":100000}, {"Billing Date":"2025-02-01", "Units Used":100000}]}
        void loadFromFile() {
            rapidjson::Document doc;
            std::ifstream ifs(saveFile);
            std::cout << "Loading from file: " << saveFile << std::endl;
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            doc.Parse(str.c_str());

            for (auto &consumer : doc.GetArray()) {
                if(consumer.IsObject()) {
                    std::vector<Billing::History> history;
                    if(consumer.HasMember("name") && consumer["name"].IsString() &&
                       consumer.HasMember("id") && consumer["id"].IsInt() &&
                       consumer.HasMember("address") && consumer["address"].IsString() &&
                       consumer.HasMember("data") && consumer["data"].IsArray()) {
                        
                        const auto& dataArray = consumer["data"].GetArray();
                            for(const auto& billing : dataArray) {
                                if(billing.IsObject() && 
                                   billing.HasMember("Units Used") && 
                                   billing["Units Used"].IsInt() &&
                                   billing.HasMember("Billing Date") &&
                                   billing["Billing Date"].IsString()) {
            
                                    history.push_back(Billing::History(billing["Units Used"].GetInt(), billing["Billing Date"].GetString()));
                                }
                            }
                        }
                        consumers[consumer["id"].GetInt()] = Billing(Consumer(consumer["name"].GetString(), consumer["id"].GetInt(), consumer["address"].GetString()), history);
                        count++;
                    }
                }
        }
        /*
        [
	        {
                "name": "User_1425",
                "id": 1,
                "address": "Street #12, Gotham",
                "data": [
                    { "Billing Date": "2025-01-01", "Units Used": 87532 },
                    { "Billing Date": "2025-02-01", "Units Used": 89999 }
                ]
	        }
        ]
        */
        void saveToFile() {
            std::ostringstream oss;
            bool first = true;
            oss << "[" << endl;
            for (auto &consumer : consumers) {
                if(!first) {
                    oss << "," << endl;
                }
                oss << "\t{" << endl;
                oss << "\t\t\"name\":\"" << consumer.second.consumer.name << "\"," << endl;
                oss << "\t\t\"id\":" << consumer.second.consumer.id << "," << endl;
                oss << "\t\t\"address\":\"" << consumer.second.consumer.address << "\"," << endl;
                oss << "\t\t\"data\":[" << endl;
                bool firstHistory = true;
                for(const auto& history : consumer.second.history) {
                    if(!firstHistory) {
                        oss << "," << endl;
                    }
                    oss << "\t\t\t{ \"Billing Date\":\"" << history.billingDate << "\", \"Units Used\":" << history.unitsUsed << " }";
                    firstHistory = false;
                }
                oss << endl << "\t\t]" << endl;
                oss << "\t}";
                first = false;
            }
            oss << endl << "]";
            std::ofstream ofs(saveFile);
            ofs << oss.str();
            ofs.flush();
            ofs.close();
        }

        public:
            EMS(const string &filename) : saveFile(filename) {
                loadFromFile();
            }

            void addConsumer(string name, int id, string address, int unitsUsed, string billingDate) {
                auto it = consumers.find(id);
                if(it != consumers.end()) {
                    bool dateExists = false;
                    for(const auto& history : it->second.history) {
                        if(history.billingDate == billingDate) {
                            dateExists = true;
                            break;
                        }
                    }                    
                    if(dateExists) {
                        cout << "Data for consumer with ID " << id << " for this month already exists." << endl;
                        return;
                    } else {
                        it->second.history.push_back(Billing::History(unitsUsed, billingDate));
                        cout << "New billing data added for consumer with ID " << id << "." << endl;
                    }
                } else {
                    std::vector<Billing::History> history;
                    history.push_back(Billing::History(unitsUsed, billingDate));
                    consumers[id] = Billing(Consumer(name, id, address), history);
                    count++;
                    cout << "Consumer with ID " << id << " added successfully." << endl;
                }
                revenueGenerated = false;
            }

            void viewAllConsumers() {
                cout << "Name\tID\tAddress\tUnits Used" << endl;
                for (auto &consumer : consumers) {
                    cout << consumer.second << endl;
                }
            }

            void searchConsumerByID(int id) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    cout << itr->second << endl;
                }
                else {
                    cout << "Consumer with ID " << id << " not found." << endl;
                }
            }

            void removeConsumer(int id) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    consumers.erase(itr );
                    cout << "Consumer with ID " << id << " removed successfully." << endl;
                }
                else {
                    cout << "Consumer with ID " << id << " not found." << endl;
                }
            }

            void calculateBill(int id, string billingDate) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    auto rate = itr->second.getRate(billingDate);
                    int bill = rate.first * rate.second;
                    cout << "Bill for consumer with ID " << id << " is " << bill << endl;
                }
                else {
                    cout << "Consumer with ID " << id << " not found." << endl;
                }
            }

            void generateRevenue() {
                if(!revenueGenerated) {    
                    revenue = 0;
                    for (auto &consumer : consumers) {
                        auto rate = consumer.second.getRate();
                        revenue += rate.first * rate.second;
                    }
                    revenueGenerated = true;
                }
                cout << "Total revenue is " << revenue << endl;
            }

        ~EMS() {
            saveToFile();
        }
    };
    int EMS::count = 0;
}

#endif