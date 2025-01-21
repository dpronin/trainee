#include <iostream>
#include <vector>
#include <algorithm>

#include <boost/container/static_vector.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/range/algorithm/adjacent_find.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/sort.hpp>

struct point {
	int x;
	int y;
};

std::ostream& operator<<(std::ostream &out, point const &p)
{
	out << "{" << p.x << " : " << p.y << "}";
	return out;
}

int main(int argc, char const *argv[]) {
	boost::ignore_unused(argc);
	boost::ignore_unused(argv);
	boost::container::static_vector<std::vector<point>, 10> vectors = {{
		  { { 1, 2 }, { -2, 5 }, { -8, -19 }, { -8, 0 } }
		, { { -9, 0 }, { 12, 6 } }
		, { { 1, -9 } }
		, { { 2, 1 } }
		, { { 1, 2 }, { -2, 0 }, { -8, 19 }, { 8, 0 } }
		, {}
	}};

	for (auto&& v : vectors) {
		boost::sort(v, [](point const &left, point const &right){ return left.x < right.x; });
		boost::copy(v, std::ostream_iterator<std::remove_reference_t<decltype(v)>::value_type const&>(std::cout, " "));
		std::cout << std::endl;
		BOOST_TEST(boost::adjacent_find(v, [](point const &left, point const &right){ return right.x < left.x; }) == v.end());
	}

	return boost::report_errors();
}
