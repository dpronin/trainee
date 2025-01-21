#include <iostream>

#include <boost/chrono.hpp>

int main(int argc, char const *argv[]) {
    std::cout << "0 " << boost::chrono::system_clock::to_time_t(boost::chrono::system_clock::now()) << '\n';
    std::cout << "1 " << boost::chrono::system_clock::now() << '\n';
    std::cout << "2 " << boost::chrono::steady_clock::now() << '\n';
    std::cout << "3 " << boost::chrono::high_resolution_clock::now() << '\n';
    std::cout << "4 " << boost::chrono::process_real_cpu_clock::now() << '\n';
    std::cout << "5 " << boost::chrono::process_user_cpu_clock::now() << '\n';
    std::cout << "6 " << boost::chrono::process_system_cpu_clock::now() << '\n';
    std::cout << "7 " << boost::chrono::process_cpu_clock::now() << '\n';
    std::cout << "8 " << boost::chrono::thread_clock::now() << '\n';

    return 0;
}
