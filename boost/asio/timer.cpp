#include <iostream>
#include <chrono>

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/system_error.hpp>

using namespace std::literals;

boost::asio::io_service iosr;
boost::asio::io_service::work work(iosr);
boost::asio::steady_timer mytimer{iosr};

void printer()
{
	std::cout << "timeout triggered, 3 seconds passed..." << std::endl;
}

void handler(boost::system::system_error const &er);

void PeekTimer(boost::asio::steady_timer &mytimer) {
	mytimer.expires_from_now(3s);
	mytimer.async_wait(handler);
}

void handler(boost::system::system_error const &er){
	printer();
	PeekTimer(mytimer);
};

int main(int argc, char const *argv[]) {
	PeekTimer(mytimer);
	iosr.run();
	std::cout << "exit program" << std::endl;
	return 0;
}
