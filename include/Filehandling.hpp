#ifndef FILEHANDLING_HPP
#define FILEHANDLING_HPP

#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <rapidjson/document.h>
#include "Billing.hpp"

namespace DataHandling {

    class FileHandler {
        std::string saveFile;
        public:
            FileHandler(const std::string& filename) : saveFile(filename) {}
            
            void loadData(std::unordered_map<int, CORE::Billing>& consumers) {
                int count = 0;
                rapidjson::Document doc;
                std::ifstream ifs(saveFile);
                std::cout << "Loading from file: " << saveFile << std::endl;
                std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                doc.Parse(str.c_str());

                for (auto &consumer : doc.GetArray()) {
                    if(consumer.IsObject()) {
                        std::vector<CORE::Billing::History> history;
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
                
                                        history.push_back(CORE::Billing::History(billing["Units Used"].GetInt(), billing["Billing Date"].GetString()));
                                    }
                                }
                            }
                            consumers[consumer["id"].GetInt()] = CORE::Billing(CORE::Consumer(consumer["name"].GetString(), consumer["id"].GetInt(), consumer["address"].GetString()), history);
                            count++;
                        }
                    }
            }

            void saveData(std::unordered_map<int, CORE::Billing>& consumers) {
                std::ostringstream oss;
                bool first = true;
                oss << "[" << std::endl;
                for (auto &consumer : consumers) {
                    if(!first) {
                        oss << "," << std::endl;
                    }
                    oss << "\t{" << std::endl;
                    oss << "\t\t\"name\":\"" << consumer.second.consumer.name << "\"," << std::endl;
                    oss << "\t\t\"id\":" << consumer.second.consumer.id << "," << std::endl;
                    oss << "\t\t\"address\":\"" << consumer.second.consumer.address << "\"," << std::endl;
                    oss << "\t\t\"data\":[" << std::endl;
                    bool firstHistory = true;
                    for(const auto& history : consumer.second.history) {
                        if(!firstHistory) {
                            oss << "," << std::endl;
                        }
                        oss << "\t\t\t{ \"Billing Date\":\"" << history.billingDate << "\", \"Units Used\":" << history.unitsUsed << " }";
                        firstHistory = false;
                    }
                    oss << std::endl << "\t\t]" << std::endl;
                    oss << "\t}";
                    first = false;
                }
                oss << std::endl << "]";
                std::ofstream ofs(saveFile);
                ofs << oss.str();
                ofs.flush();
                ofs.close();
            }
    };
}

#endif