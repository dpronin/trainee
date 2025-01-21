#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

#include <boost/core/ignore_unused.hpp>

int main(int argc, char const *argv[]) {
	boost::ignore_unused(argc);
	boost::ignore_unused(argv);

	std::list<int> l = {1, 2, 3};
	std::vector<int> v = {-1, -2, -3, -4, 5};

	std::swap_ranges(l.begin(), l.end(), v.begin());

	std::cout << "list" << std::endl;;
	for (auto&& item : l) {
		std::cout << item << " ";
	}
	std::cout << std::endl;

	std::cout << "vector" << std::endl;;
	for (auto&& item : v) {
		std::cout << item << " ";
	}
	std::cout << std::endl;
	return 0;
}
