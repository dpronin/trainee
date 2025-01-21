#include <iostream>
#include <chrono>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/system_error.hpp>

using namespace std::literals;

int main(int argc, char const *argv[]) {
    boost::asio::io_service io_service;
    boost::asio::io_service::work work(io_service);

    io_service.post([]{
        std::cout << "hello" << std::endl;
        std::this_thread::sleep_for(1s);
    });

    io_service.post([]{
        std::cout << ", " << std::endl;
        std::this_thread::sleep_for(1s);
    });

    io_service.post([]{
        std::cout << "world" << std::endl;
        std::this_thread::sleep_for(1s);
    });

    auto t = std::thread([&]{ io_service.run(); });
    t.join();
    return 0;
}
