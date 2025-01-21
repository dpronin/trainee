#include <iostream>

#include <boost/timer/timer.hpp>

#include <vector>

#include <boost/container/vector.hpp>

constexpr auto COUNT = 100000000;

int main(int argc, char const *argv[]) {
    std::vector<int> v, v1;
    boost::container::vector<int> bv;

    boost::timer::cpu_timer timer;

    for (int i = 0; i < COUNT; ++i) bv.push_back(i);
    std::cout << "boost::container::vector worked: " << timer.format() << std::endl;

    timer.start();

    for (int i = 0; i < COUNT; ++i) v.push_back(i);
    std::cout << "std::vector worked: " << timer.format() << std::endl;

    return 0;
}
