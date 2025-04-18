#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include "Billing.hpp"
#include <memory>

namespace DataHandling {
    class Database {
        std::string host;
        std::string user;
        std::string password;
        std::string database;
        std::unique_ptr<sql::Connection> conn;
        public:
            Database(std::string host, std::string user, std::string password, std::string database) {
                this->host = host;
                this->user = user;
                this->password = password;
                this->database = database;
                connect();
            }
            
            Database(Database&& other) noexcept 
                : host(std::move(other.host)), 
                  user(std::move(other.user)), 
                  password(std::move(other.password)), 
                  database(std::move(other.database)),
                  conn(std::move(other.conn)) {}
            
            Database& operator=(Database&& other) noexcept {
                if (this != &other) {
                    host = std::move(other.host);
                    user = std::move(other.user);
                    password = std::move(other.password);
                    database = std::move(other.database);
                    conn = std::move(other.conn);
                }
                return *this;
            }
            
            ~Database() {
                disconnect();
            }
            
            void connect() {
                try {
                    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                    conn = std::unique_ptr<sql::Connection>(driver->connect(host, user, password));
                    conn->setSchema(database);
                } catch (sql::SQLException& e) {
                    std::cerr << "MySQL error: " << e.what() << std::endl;
                }
            }
            
            bool saveData(const int& id, const std::string& name, const std::string& address, const std::string& billingDate, const int& reading) {
                try {
                    sql::Statement* stmt = conn->createStatement();
                    stmt->execute("INSERT INTO Consumers (id, name, address) VALUES (" + std::to_string(id) + ", '" + name + "', '" + address + "')");
                    stmt->execute("INSERT INTO Billing (consumer_id, billing_date, units_used) VALUES (" + std::to_string(id) + ", '" + billingDate + "', " + std::to_string(reading) + ")");
                    delete stmt;
                    return true;
                } catch (sql::SQLException& e) {
                    std::cerr << "MySQL error: " << e.what() << std::endl;
                    return false;
                }
            }

            bool saveData(std::unordered_map<int, CORE::Billing>& consumers) {
                try {
                    for (auto& pair : consumers) {
                        int id = pair.first;
                        CORE::Billing& billing = pair.second;
                        
                        try {
                            sql::Statement* stmt = conn->createStatement();
                            stmt->execute("INSERT INTO Consumers (id, name, address) VALUES (" + 
                                std::to_string(id) + ", '" + 
                                billing.consumer.name + "', '" + 
                                billing.consumer.address + "')");
                            delete stmt;
                        } catch (sql::SQLException& e) {
                            sql::Statement* stmt = conn->createStatement();
                            stmt->execute("UPDATE Consumers SET name='" + 
                                billing.consumer.name + "', address='" + 
                                billing.consumer.address + "' WHERE id=" + 
                                std::to_string(id));
                            delete stmt;
                        }
                        
                        for (const auto& history : billing.history) {
                            try {
                                sql::Statement* stmt = conn->createStatement();
                                stmt->execute("INSERT INTO Billing (consumer_id, billing_date, units_used) VALUES (" + 
                                    std::to_string(id) + ", '" + 
                                    history.billingDate + "', " + 
                                    std::to_string(history.unitsUsed) + ")");
                                delete stmt;
                            } catch (sql::SQLException& e) {
                                sql::Statement* stmt = conn->createStatement();
                                stmt->execute("UPDATE Billing SET units_used=" + 
                                    std::to_string(history.unitsUsed) + 
                                    " WHERE consumer_id=" + std::to_string(id) + 
                                    " AND billing_date='" + history.billingDate + "'");
                                delete stmt;
                            }
                        }
                    }
                    return true;
                } catch (sql::SQLException& e) {
                    std::cerr << "MySQL error while saving data: " << e.what() << std::endl;
                    return false;
                }
            }

            bool loadData(std::unordered_map<int, CORE::Billing>& consumers) {
                int count = 0;
                try {
                    std::unique_ptr<sql::Statement> stmt(conn->createStatement());
                    
                    std::unique_ptr<sql::ResultSet> consumerRes(
                        stmt->executeQuery("SELECT id, name, address FROM Consumers")
                    );
                    
                    while (consumerRes->next()) {
                        int id = consumerRes->getInt("id");
                        std::string name = consumerRes->getString("name");
                        std::string address = consumerRes->getString("address");
                        
                        std::unique_ptr<sql::PreparedStatement> billingStmt(
                            conn->prepareStatement("SELECT billing_date, units_used FROM Billing WHERE consumer_id = ?")
                        );
                        billingStmt->setInt(1, id);
                        std::unique_ptr<sql::ResultSet> billingRes(billingStmt->executeQuery());
                        
                        std::vector<CORE::Billing::History> history;
                        while (billingRes->next()) {
                            std::string billingDate = billingRes->getString("billing_date");
                            int unitsUsed = billingRes->getInt("units_used");
                            history.push_back(CORE::Billing::History(unitsUsed, billingDate));
                        }
                        
                        consumers[id] = CORE::Billing(CORE::Consumer(name, id, address), history);
                        count++;
                    }
                    
                    return true;
                } catch (sql::SQLException& e) {
                    std::cerr << "MySQL error while loading data: " << e.what() << std::endl;
                    return false;
                }
            }

            void disconnect() {
                conn.reset();
            }
    };
}