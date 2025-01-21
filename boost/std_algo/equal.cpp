#include <vector>
#include <list>
#include <algorithm>
#include <iostream>

#include <boost/core/ignore_unused.hpp>

int main(int argc, char const *argv[]) {
	boost::ignore_unused(argc);
	boost::ignore_unused(argv);
	std::vector<int> v = { 1, 2, 3, 4 };
	std::list<int> l = { 1, 2, 3, 4, 5, 6, 7 };
	std::cout << std::boolalpha << std::equal(v.begin(), v.end(), l.begin()) << std::endl;
	return 0;
}
