#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <map>
#include <sstream>
#include <functional>

namespace ssl = boost::asio::ssl;
using boost::asio::ip::tcp;

namespace Network {
    // Forward declaration for Route handler function type
    class Session;
    using RouteHandler = std::function<void(std::shared_ptr<Session>, const std::string&, const std::map<std::string, std::string>&)>;
    
    class Session : public std::enable_shared_from_this<Session> {
        ssl::stream<tcp::socket> socket_;
        boost::asio::streambuf buffer_;
        std::string request_method_;
        std::string request_uri_;
        std::string request_version_;
        std::map<std::string, std::string> request_headers_;
        std::map<std::string, std::string> query_params_;
        
        // Static route tables for different HTTP methods
        static std::map<std::string, RouteHandler> get_routes_;
        static std::map<std::string, RouteHandler> post_routes_;
        static std::map<std::string, RouteHandler> put_routes_;
        static std::map<std::string, RouteHandler> delete_routes_;

        void do_handshake() {
            auto self(shared_from_this());
            socket_.async_handshake(ssl::stream_base::server,
            [this, self](boost::system::error_code ec) {
                if (!ec) {
                    do_read();
                }
                else {
                    std::cerr << "Handshake failed: " << ec.message() << std::endl;
                }
            });
        }
        void do_read() {
            auto self = shared_from_this();
            boost::asio::async_read_until(socket_, buffer_, "\r\n\r\n",
                [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        parse_request();
                        handle_request();
                    } else {
                        std::cerr << "Read failed: " << ec.message() << "\n";
                    }
                });
        }
        void parse_request() {
            std::istream request_stream(&buffer_);
            std::string request_line;
            std::getline(request_stream, request_line);
            
            if (!request_line.empty() && request_line.back() == '\r') {
                request_line.pop_back();
            }
            
            std::istringstream request_line_stream(request_line);
            request_line_stream >> request_method_ >> request_uri_ >> request_version_;
            
            parse_query_parameters();
            
            std::string header_line;
            while (std::getline(request_stream, header_line) && header_line != "\r") {
                if (header_line.back() == '\r') {
                    header_line.pop_back();
                }
                
                auto separator = header_line.find(": ");
                if (separator != std::string::npos) {
                    std::string header_name = header_line.substr(0, separator);
                    std::string header_value = header_line.substr(separator + 2);
                    request_headers_[header_name] = header_value;
                }
            }
        }
        void parse_query_parameters() {
            size_t query_pos = request_uri_.find('?');
            if (query_pos != std::string::npos) {
                std::string query_string = request_uri_.substr(query_pos + 1);
                request_uri_ = request_uri_.substr(0, query_pos);
                
                size_t start = 0;
                size_t end;
                while ((end = query_string.find('&', start)) != std::string::npos) {
                    parse_query_param(query_string.substr(start, end - start));
                    start = end + 1;
                }
                if (start < query_string.length()) {
                    parse_query_param(query_string.substr(start));
                }
            }
        }
        void parse_query_param(const std::string& param) {
            size_t equals_pos = param.find('=');
            if (equals_pos != std::string::npos) {
                std::string key = param.substr(0, equals_pos);
                std::string value = param.substr(equals_pos + 1);
                query_params_[key] = value;
            } else {
                query_params_[param] = "";
            }
        }
        void handle_request() {
            if (request_method_ == "GET") {
                handle_route(get_routes_);
            } else if (request_method_ == "POST") {
                handle_route(post_routes_);
            } else if (request_method_ == "PUT") {
                handle_route(put_routes_);
            } else if (request_method_ == "DELETE") {
                handle_route(delete_routes_);
            } else {
                handle_unsupported_method();
            }
        }
        void handle_route(const std::map<std::string, RouteHandler>& routes) {
            auto route = routes.find(request_uri_);
            if (route != routes.end()) {
                route->second(shared_from_this(), request_uri_, request_headers_);
            } else {
                std::string response =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 22\r\n\r\n"
                    "Route not found: " + request_uri_;
                
                send_response(response);
            }
        }
        void handle_unsupported_method() {
            std::string response =
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 29\r\n\r\n"
                "Method not supported: " + request_method_;
            
            send_response(response);
        }
        public:
            Session(tcp::socket socket, boost::asio::ssl::context& context) : socket_(std::move(socket), context){}

            void start() {
                do_handshake();
            }            
            
            void send_response(const std::string& response) {
                auto self = shared_from_this();
                boost::asio::async_write(socket_, boost::asio::buffer(response),
                    [this, self](const boost::system::error_code& ec, std::size_t length) {
                        if (ec) {
                            std::cerr << "Write failed: " << ec.message() << "\n";
                        }
                    });
            }
            // Static methods to register routes
            static void register_get(const std::string& path, RouteHandler handler) {
                get_routes_[path] = handler;
            }
            
            static void register_post(const std::string& path, RouteHandler handler) {
                post_routes_[path] = handler;
            }
            
            static void register_put(const std::string& path, RouteHandler handler) {
                put_routes_[path] = handler;
            }
            
            static void register_delete(const std::string& path, RouteHandler handler) {
                delete_routes_[path] = handler;
            }
            
            const std::map<std::string, std::string>& get_query_params() const {
                return query_params_;
            }
            
            std::string get_query_param(const std::string& name, const std::string& default_value = "") const {
                auto it = query_params_.find(name);
                if (it != query_params_.end()) {
                    return it->second;
                }
                return default_value;
            }

            int get_queries_count() const {
                return query_params_.size();
            }
    };
    
    // Initialize static route tables
    std::map<std::string, RouteHandler> Session::get_routes_;
    std::map<std::string, RouteHandler> Session::post_routes_;
    std::map<std::string, RouteHandler> Session::put_routes_;
    std::map<std::string, RouteHandler> Session::delete_routes_;
}
