#include <iostream>
#include "CServer.h"
int main(){
	try {
		unsigned short port_num = static_cast<unsigned short>(8080);
		net::io_context ioc{ 1 };
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return;
			}
			ioc.stop();
			});
		std::make_shared<CServer>(ioc, port_num)->start();
		std::cout << "Gate Server listen on port: " << port_num << std::endl;
		ioc.run();
   }
	catch (std::exception const& e) {
		std::cerr << "Error is :" << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}