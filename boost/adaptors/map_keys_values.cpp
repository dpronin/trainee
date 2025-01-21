#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <array>
#include <map>
#include <string>
#include <utility>
#include <iterator>
#include <iostream>

int main()
{
    using namespace boost::adaptors;

    std::array<int, 3> a{{0, 1, 2}};
    std::map<std::string, int*> m;
    m.insert(std::make_pair("a", &a[0]));
    m.insert(std::make_pair("b", &a[1]));
    m.insert(std::make_pair("c", &a[2]));

    boost::copy(m | map_keys, std::ostream_iterator<std::string>{std::cout, ","});
    boost::copy(m | map_values | indirected, std::ostream_iterator<int>{std::cout, ","});

    return 0;
}
