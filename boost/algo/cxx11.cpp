#include <vector>
#include <iostream>
#include <iterator>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/clamp.hpp>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/transformed.hpp>

int main(int argc, char const *argv[]) {
    std::vector<int> v = {1, 2, 5, 6, 7, 8};
    std::cout << std::boolalpha << "boost::algorithm::all_of(v, [](int a){ return a < 9; }) " << boost::algorithm::all_of(v, [](int a){ return a < 9; }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::all_of(v, [](int a){ return a % 2; }) " << boost::algorithm::all_of(v, [](int a){ return a % 2; }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::all_of(v, [](int a){ return !(a % 2); }) " << boost::algorithm::all_of(v, [](int a){ return !(a % 2); }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::all_of(v, [](int a){ return a != 0; }) " << boost::algorithm::all_of(v, [](int a){ return a != 0; }) << std::noboolalpha << std::endl;

    std::cout << std::boolalpha << "boost::algorithm::any_of(v, [](int a){ return a < 9; }) " << boost::algorithm::any_of(v, [](int a){ return a < 9; }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::any_of(v, [](int a){ return a % 2; }) " << boost::algorithm::any_of(v, [](int a){ return a % 2; }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::any_of(v, [](int a){ return !(a % 2); }) " << boost::algorithm::any_of(v, [](int a){ return !(a % 2); }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::any_of(v, [](int a){ return a != 0; }) " << boost::algorithm::any_of(v, [](int a){ return a != 0; }) << std::noboolalpha << std::endl;

    std::cout << std::boolalpha << "boost::algorithm::none_of(v, [](int a){ return a < 9; }) " << boost::algorithm::none_of(v, [](int a){ return a < 9; }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::none_of(v, [](int a){ return a % 2; }) " << boost::algorithm::none_of(v, [](int a){ return a % 2; }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::none_of(v, [](int a){ return !(a % 2); }) " << boost::algorithm::none_of(v, [](int a){ return !(a % 2); }) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::none_of(v, [](int a){ return a != 0; }) " << boost::algorithm::none_of(v, [](int a){ return a != 0; }) << std::noboolalpha << std::endl;

    std::cout << std::boolalpha << "boost::algorithm::is_sorted(v) " << boost::algorithm::is_sorted(v) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::is_increasing(v) " << boost::algorithm::is_increasing(v) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::is_decreasing(v) " << boost::algorithm::is_decreasing(v) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::is_strictly_increasing(v) " << boost::algorithm::is_strictly_increasing(v) << std::noboolalpha << std::endl;
    std::cout << std::boolalpha << "boost::algorithm::is_strictly_decreasing(v) " << boost::algorithm::is_strictly_decreasing(v) << std::noboolalpha << std::endl;

    using namespace boost::adaptors;
    std::cout << "boost::range::copy(clamp): ";
    boost::range::copy(v | transformed([](int a){ return boost::algorithm::clamp(a, 5, 6); }), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    std::cout << "boost::algorithm::clamp_range: ";
    boost::algorithm::clamp_range(v, std::ostream_iterator<int>(std::cout, " "), 5, 6);
    std::cout << std::endl;
    return 0;
}
