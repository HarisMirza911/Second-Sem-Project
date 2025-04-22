#include "Session.hpp"

namespace Network {
    class Server {
        public:
            Server(boost::asio::io_context& io_context, ssl::context& ssl_context, unsigned short port)
                : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), ssl_context_(ssl_context) {}

            void start() {
                std::cout << "Server starting on port " << acceptor_.local_endpoint().port() << std::endl;
                do_accept();
            }

        private:
            void do_accept() {
                acceptor_.async_accept(
                    [this](const boost::system::error_code& ec, tcp::socket socket) {
                        if (!ec) {
                            std::make_shared<Session>(std::move(socket), ssl_context_)->start();
                        } else {
                            std::cerr << "Accept failed: " << ec.message() << "\n";
                        }
                        do_accept();
                    });
            }

            tcp::acceptor acceptor_;
            ssl::context& ssl_context_;
        };
}