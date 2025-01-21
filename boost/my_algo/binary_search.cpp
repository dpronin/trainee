#include <iostream>
#include <utility>
#include <vector>

#include <boost/container/static_vector.hpp>
#include <boost/core/ignore_unused.hpp>

#include "algo.hpp"

int main(int argc, char const *argv[]) {
	boost::ignore_unused(argc);
	boost::ignore_unused(argv);
	boost::container::static_vector<std::vector<int>, 10> vectors = {{
		  { 1, 2, 2, 4, 10, 12, 13, 50 }
		, { -9, 0, 1, 6 }
		, { 1 }
		, { 1, 1, 2 }
		, { -1, -2, -2, 0, 8, 19, 88, 100 }
		, {}
	}};

	for (auto&& v : vectors) {
		auto it = algo::up_bound(v.begin(), v.end(), 0);
		if (it == v.end()) std::cout << "no item 0";
		else std::cout << "index of item 0 is " << std::distance(v.begin(), it);
		std::cout << std::endl;
	}

	return 0;
}
