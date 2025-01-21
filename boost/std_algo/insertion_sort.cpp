#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/range/algorithm/upper_bound.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/core/ignore_unused.hpp>

int main(int argc, char const *argv[]) {
	boost::ignore_unused(argc);
	boost::ignore_unused(argv);
	std::vector<int> v{2, 4, 2, 0, 5, 10, 7, 3, 7, 1};

	std::cout << "before sort: ";
	for (int n: v) std::cout << n << ' ';
	std::cout << '\n';

	// insertion sort
	for (auto i = v.begin(); i != v.end(); ++i) {
		std::rotate(boost::upper_bound(boost::make_iterator_range(v.begin(), i), *i), i, i+1);
	}

	std::cout << "upon sort: ";
	for (int n : v) std::cout << n << ' ';
	std::cout << '\n';

	return 0;
}
