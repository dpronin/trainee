#include <vector>
#include <iostream>
#include <iterator>

#include <boost/range/algorithm/lower_bound.hpp>
#include <boost/range/algorithm/upper_bound.hpp>

#include <boost/core/ignore_unused.hpp>

int main(int argc, char const *argv[]) {
	boost::ignore_unused(argc);
	boost::ignore_unused(argv);
	std::vector<int> v { 1, 1, 2, 3, 4, 4, 4, 4, 5, 6, 7, 8, 8, 8, 9 };
	auto beg = boost::lower_bound(v, 4, [](int item, int val){ return item < val; });
	auto fin = boost::upper_bound(v, 8, [](int val, int item){ return val < item; });
	std::copy(beg, fin, std::ostream_iterator<int>(std::cout, " "));
	return 0;
}
