#ifndef EMS_HPP
#define EMS_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Billing.hpp"
#include "Filehandling.hpp"
#include "Database.hpp"

namespace CORE {
    template<typename SAVETYPE>
    class EMS {
        static int count;
        std::unordered_map<int, Billing> consumers;
        bool revenueGenerated = false;
        int revenue = 0;
        std::string saveFile;
        SAVETYPE saveHandler;

        public:
            EMS(const std::string &filename, SAVETYPE&& saveHandler) 
                : saveFile(filename), saveHandler(std::move(saveHandler)) {
                this->saveHandler.loadData(consumers);
            }

            void addConsumer(std::string name, int id, std::string address, int unitsUsed, std::string billingDate) {
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
                        std::cout << "Data for consumer with ID " << id << " for this month already exists." << std::endl;
                        return;
                    } else {
                        it->second.history.push_back(Billing::History(unitsUsed, billingDate));
                        std::cout << "New billing data added for consumer with ID " << id << "." << std::endl;
                    }
                } else {
                    std::vector<Billing::History> history;
                    history.push_back(Billing::History(unitsUsed, billingDate));
                    consumers[id] = Billing(Consumer(name, id, address), history);
                    count++;
                    std::cout << "Consumer with ID " << id << " added successfully." << std::endl;
                }
                revenueGenerated = false;
            }

            void viewAllConsumers() {
                std::cout << "Name\tID\tAddress\tUnits Used" << std::endl;
                for (auto &consumer : consumers) {
                    std::cout << consumer.second << std::endl;
                }
            }

            void searchConsumerByID(int id) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    std::cout << itr->second << std::endl;
                }
                else {
                    std::cout << "Consumer with ID " << id << " not found." << std::endl;
                }
            }

            void removeConsumer(int id) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    consumers.erase(itr);
                    std::cout << "Consumer with ID " << id << " removed successfully." << std::endl;
                }
                else {
                    std::cout << "Consumer with ID " << id << " not found." << std::endl;
                }
            }

            void calculateBill(int id, std::string billingDate) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    auto rate = itr->second.getRate(billingDate);
                    int bill = rate.first * rate.second;
                    std::cout << "Bill for consumer with ID " << id << " is " << bill << std::endl;
                }
                else {
                    std::cout << "Consumer with ID " << id << " not found." << std::endl;
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
                std::cout << "Total revenue is " << revenue << std::endl;
            }

        ~EMS() {
            saveHandler.saveData(consumers);
        }
    };
    template<typename SAVETYPE>
    int EMS<SAVETYPE>::count = 0;
}

#endif