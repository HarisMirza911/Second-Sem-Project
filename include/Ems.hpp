#ifndef EMS_HPP
#define EMS_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Billing.hpp"
#include "Filehandling.hpp"
#include "Database.hpp"
#include "Server.hpp"
// std::function<void(std::shared_ptr<Session>, const std::string&, const std::map<std::string, std::string>&)>;
namespace CORE {
    template<typename SAVETYPE>
    class EMS {
        static int count;
        std::unordered_map<int, Billing> consumers;
        bool revenueGenerated = false;
        int revenue = 0;
        std::string saveFile;
        SAVETYPE saveHandler;
        Network::Server& server;

        void onGetBill(std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
            std::cout << "Processing bill request..." << std::endl;
            std::string id = session->get_query_param("id", "unknown");
            std::string date = session->get_query_param("date", "unknown");
            std::cout << "Request parameters - ID: " << id << ", Date: " << date << std::endl;
            
            // Add error handling for invalid parameters
            if (id == "unknown" || date == "unknown") {
                std::string response_body = "{\"error\": \"Missing required parameters: id and date\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
                return;
            }
            
            try {
                int consumer_id = std::stoi(id);
                std::cout << "Looking for consumer with ID: " << consumer_id << std::endl;
                
                // Check if consumer exists
                if (consumers.find(consumer_id) == consumers.end()) {
                    std::cout << "Consumer not found" << std::endl;
                    std::string response_body = "{\"error\": \"Consumer with ID " + id + " not found\"}";
                    std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                    session->send_response(response);
                    return;
                }
                
                std::cout << "Consumer found, calculating bill..." << std::endl;
                int bill = calculateBill(consumer_id, date);
                std::string name = consumers[consumer_id].consumer.name;
                std::string address = consumers[consumer_id].consumer.address;
                std::string response_body = "{\"message\": \"Bill details\", \"id\": \"" + id + "\", \"name\": \"" + name + "\", \"address\": \"" + address + "\", \"date\": \"" + date + "\", \"bill\": " + std::to_string(bill) + "}";
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
            } catch (const std::exception& e) {
                std::cout << "Exception: " << e.what() << std::endl;
                std::string response_body = "{\"error\": \"Invalid ID format\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
            }
        }

        void onGetConsumer(std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
            std::cout << "Processing consumer request..." << std::endl;
            std::string id = session->get_query_param("id", "unknown");
            std::cout << "Request parameters - ID: " << id << std::endl;
            // [{...}, {...}, {...}]
            if(id == "unknown") {
                std::string response_body = "[";
                for(const auto& consumer : consumers) {
                    response_body += "{\"id\": " + std::to_string(consumer.first) + ", \"name\": \"" + consumer.second.consumer.name + "\", \"address\": \"" + consumer.second.consumer.address + "\"},";
                }
                response_body += "\b]";
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
            }

            try {
                int consumer_id = std::stoi(id);
                std::cout << "Looking for consumer with ID: " << consumer_id << std::endl;
                
                if(consumers.find(consumer_id) == consumers.end()) {
                    std::cout << "Consumer not found" << std::endl;
                    std::string response_body = "{\"error\": \"Consumer with ID " + id + " not found\"}";
                    std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                    session->send_response(response);
                    return;
                }
                
                std::cout << "Consumer found, sending response..." << std::endl;
                std::string name = consumers[consumer_id].consumer.name;
                std::string address = consumers[consumer_id].consumer.address;
                std::string response_body = "{\"message\": \"Consumer details\", \"id\": \"" + id + "\", \"name\": \"" + name + "\", \"address\": \"" + address + "\"}";
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
            }
            catch (const std::exception& e) {
                std::cout << "Exception: " << e.what() << std::endl;
                std::string response_body = "{\"error\": \"Invalid ID format\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
            }
            
            
        }

        void onGetRevenue(std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
            std::cout << "Processing revenue request..." << std::endl;
            std::string response_body = "{\"message\": \"Revenue details\", \"revenue\": " + std::to_string(revenue) + "}";
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
            session->send_response(response);
        }

        void onPostConsumer(std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
            // std::string name, int id, std::string address, int unitsUsed, std::string billingDate
            std::cout << "Processing consumer request..." << std::endl;
            std::string id = session->get_query_param("id");
            std::string name = session->get_query_param("name");
            std::string address = session->get_query_param("address");
            std::string unitsUsed = session->get_query_param("unitsUsed");
            std::string billingDate = session->get_query_param("billingDate");
            std::string response_body = "";
            if (name.empty()) {
                response_body = "{\"error\": \"Name is empty\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
                return;
            }
            if (id.empty()) {
                response_body = "{\"error\": \"ID is empty\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
                return;
            }
            if (address.empty()) {
                response_body = "{\"error\": \"Address is empty\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
                return;
            }
            if (unitsUsed.empty()) {
                response_body = "{\"error\": \"Units used is empty\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
                return;
            }
            if (billingDate.empty()) {
                response_body = "{\"error\": \"Billing date is empty\"}";
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
                session->send_response(response);
                return;
            }
            addConsumer(name, std::stoi(id), address, std::stoi(unitsUsed), billingDate);
            response_body = "{\"message\": \"Consumer added successfully with name " + name + " and id " + id + "\"}";
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" + response_body;
            session->send_response(response);
        }
        public:
            EMS(const std::string &filename, SAVETYPE&& saveHandler, Network::Server& server) 
                : saveFile(filename), saveHandler(std::move(saveHandler)), server(server) {
                this->saveHandler.loadData(consumers);
                generateRevenue();
                // Register the route handler BEFORE starting the server
                Network::Session::register_get("/bill", [this](std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
                    this->onGetBill(session, uri, headers);
                });

                Network::Session::register_get("/consumer", [this](std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
                    this->onGetConsumer(session, uri, headers);
                });
                
                Network::Session::register_get("/revenue", [this](std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
                    this->onGetRevenue(session, uri, headers);
                });

                Network::Session::register_post("/consumer", [this](std::shared_ptr<Network::Session> session, const std::string& uri, const std::map<std::string, std::string>& headers) {
                    this->onPostConsumer(session, uri, headers);
                });

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

            int calculateBill(int id, std::string billingDate) {
                auto itr = consumers.find(id);
                if (itr != consumers.end()) {
                    auto rate = itr->second.getRate(billingDate);
                    int bill = rate.first * rate.second;
                    std::cout << "Bill for consumer with ID " << id << " is " << bill << std::endl;
                    return bill;
                }
                else {
                    std::cout << "Consumer with ID " << id << " not found." << std::endl;
                    return 0;
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

            void saveAllData() {
                std::cout << "Saving " << consumers.size() << " consumer records..." << std::endl;
                saveHandler.saveData(consumers);
                std::cout << "Data saved successfully." << std::endl;
            }

        ~EMS() {
            std::cout << "Saving Data Please Wait..." << std::endl;
            saveHandler.saveData(consumers);
        }
    };
    template<typename SAVETYPE>
    int EMS<SAVETYPE>::count = 0;
}

#endif