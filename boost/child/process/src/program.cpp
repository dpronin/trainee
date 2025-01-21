#include <thread>
#include <iostream>
#include <future>

using namespace std::literals;

int main(int argc, char const *argv[])
{
    std::cout << "Hello, world!" << std::endl;
    // auto fut = std::async(std::launch::async, []{
    //     std::cerr << "async" << std::endl;
    //     std::cin.ignore(1000, '\n');
    //     std::cerr << "lalalal" << std::endl;
    // });
    std::this_thread::sleep_for(30s);
    return 0;
}
