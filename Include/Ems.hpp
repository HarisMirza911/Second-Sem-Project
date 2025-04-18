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

        void loadFromFile() {
            rapidjson::Document doc;
            std::ifstream ifs(saveFile);
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            doc.Parse(str.c_str());
            for (auto &consumer : doc.GetArray()) {
                if(consumer.IsObject()) {
                    if(consumer.HasMember("name") && consumer["name"].IsString() &&
                    consumer.HasMember("id") && consumer["id"].IsInt() &&
                    consumer.HasMember("address") && consumer["address"].IsString() &&
                    consumer.HasMember("unitsUsed") && consumer["unitsUsed"].IsInt()
                    ) {
                        consumers[consumer["id"].GetInt()] = Billing(Consumer(consumer["name"].GetString(), consumer["id"].GetInt(), consumer["address"].GetString()), consumer["unitsUsed"].GetInt());
                        count++;
                    }
                }
            }
        }

        void saveToFile() {
            std::ostringstream oss;
            bool first = true;
            oss << "[" << endl;
            for (auto &consumer : consumers) {
                if(!first) {
                    oss << "," << endl;
                }
                oss << "\t{\"name\":\"" << consumer.second.consumer.name << "\",\"id\":" << consumer.second.consumer.id << ",\"address\":\"" << consumer.second.consumer.address << "\",\"unitsUsed\":" << consumer.second.unitsUsed << "}";
                first = false;
            }
            oss << "\n]";
            std::ofstream ofs(saveFile);
            ofs << oss.str();
            ofs.flush();
            ofs.close();
        }

        public:
            EMS(const string &filename) : saveFile(filename) {
                loadFromFile();
            }

            void addConsumer(string name, int id, string address, int unitsUsed) {
                if(consumers.find(id) != consumers.end()) {
                    cout << "Consumer with ID " << id << " already exists." << endl;
                    return;
                }
                consumers[id] = Billing(Consumer(name, id, address), unitsUsed);
                count++;
                revenueGenerated = false;
                cout << "Consumer with ID " << id << " added successfully." << endl;
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

            void calculateBill(int id) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    int unitsUsed = itr->second.unitsUsed;
                    int bill = unitsUsed * itr->second.getRate();
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
                        revenue += consumer.second.getRate() * consumer.second.unitsUsed;
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