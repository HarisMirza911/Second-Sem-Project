#include "Ems.hpp"
#include "Prometheus.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <atomic>
#include <csignal>
#include <thread>

std::unique_ptr<CORE::EMS<DataHandling::FileHandler>> fileEms;
std::unique_ptr<CORE::EMS<DataHandling::Database>> dbEms;

void signalHandler(int signum) {
    std::cout << "Interrupt Signal Received" << std::endl;
    std::cout << "Stopping the program" << std::endl;

    // Make sure to save data before resetting
    if(fileEms) {
        std::cout << "Saving file data..." << std::endl;
        // Explicitly call save method if available
        if(signum == SIGINT) {
            fileEms->handleCrash("SIGINT - Interrupt Signal Received");
        } else if(signum == SIGSEGV) {
            fileEms->handleCrash("SIGSEGV - Segmentation Fault");
        } else if(signum == SIGABRT) {
            fileEms->handleCrash("SIGABRT - Abort Signal Received");
        } else if(signum == SIGBUS) { 
            fileEms->handleCrash("SIGBUS - Bus Error");
        } else if(signum == SIGILL) {
            fileEms->handleCrash("SIGILL - Illegal Instruction");
        } else if(signum == SIGFPE) {
            fileEms->handleCrash("SIGFPE - Floating Point Exception");
        }
        fileEms->saveAllData();
        fileEms.reset();
    }

    if(dbEms) {
        std::cout << "Saving database data..." << std::endl;
        if(signum == SIGINT) {
            dbEms->handleCrash("SIGINT - Interrupt Signal Received");
        } else if(signum == SIGSEGV) {
            dbEms->handleCrash("SIGSEGV - Segmentation Fault");
        } else if(signum == SIGABRT) {
            dbEms->handleCrash("SIGABRT - Abort Signal Received");
        } else if(signum == SIGBUS) { 
            dbEms->handleCrash("SIGBUS - Bus Error");
        } else if(signum == SIGILL) {
            dbEms->handleCrash("SIGILL - Illegal Instruction");
        } else if(signum == SIGFPE) {
            dbEms->handleCrash("SIGFPE - Floating Point Exception");
        }
        dbEms->saveAllData();
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
    std::string port;
    std::string prometheusPort;
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
            port = pt.get<std::string>("PORT");
            prometheusPort = pt.get<std::string>("PROMETHEUS_PORT");
            std::cout << "Save type: " << saveType << std::endl;
            std::cout << "Save file: " << saveFile << std::endl;
            std::cout << "Port: " << port << std::endl;
        }
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    boost::asio::io_context ictx;
    ssl::context ctx(ssl::context::sslv23);
    ctx.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::single_dh_use);
    ctx.use_certificate_chain_file("./conf/server.crt");
    ctx.use_private_key_file("./conf/server.key", ssl::context::pem);
    Network::Server server(ictx, ctx, std::stoi(port));
    Monitoring::Prometheus prometheus(std::stoi(prometheusPort));

    // Create a pointer to hold our EMS instance
    if(saveType == "file") {
        fileEms = std::make_unique<CORE::EMS<DataHandling::FileHandler>>(
            saveFile, 
            std::move(DataHandling::FileHandler(saveFile)),
            server, 
            prometheus
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
            std::move(DataHandling::Database(host, user, password, database)),
            server, 
            prometheus
        );
    }
    
    // Start the server
    server.start();
    std::cout << "Server started on port " << port << std::endl;

    // Run the io_context in the main thread - this will block until all work is done
    std::cout << "Running io_context..." << std::endl;
    ictx.run();

    // This line will only be reached if io_context.run() returns, which happens when there are no more asynchronous operations
    std::cout << "Server stopped" << std::endl;

    return 0;
}
