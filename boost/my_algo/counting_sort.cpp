#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>

#include <boost/container/static_vector.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/range/algorithm/adjacent_find.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "algo.hpp"

int main(int argc, char const *argv[]) {
	boost::ignore_unused(argc);
	boost::ignore_unused(argv);
	boost::container::static_vector<std::vector<int>, 10> vectors = {{
		  {  1, 2, -2, 5, -8, -19, -8, 0 }
		, { -9, 0, 12, 6 }
		, {  1 }
		, {  2, 1 }
		, {  1, 2, -2,  0, -8, 19, 8, 0 }
		, {  0, 0, -1, -1, -1, 0 }
		, {}
	}};

	std::cout << "counting sort" << std::endl;

	for (auto&& v : vectors) {
		algo::counting_sort(v.begin(), v.end());
		boost::copy(v, std::ostream_iterator<std::remove_reference_t<decltype(v)>::value_type>(std::cout, " "));
		std::cout << std::endl;
		BOOST_TEST(boost::adjacent_find(v, [](int left, int right){ return right < left; }) == v.end());
	}

	return boost::report_errors();
}
