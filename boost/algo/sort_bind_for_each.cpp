#include <iostream>

#include <vector>

#include <boost/bind.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/for_each.hpp>

bool predicate(int left, int right) {
	return left < right;
}

int main(int argc, char const *argv[]) {
	std::vector<int> v { 10, 2, -5, 8, 0, 1, 6, -9, -10, -50, 44 };
	boost::range::for_each(v, [] (int item) { std::cout << item << " "; });
	std::cout << std::endl;
	boost::sort(v, predicate);
	boost::for_each(v, [](int item){ std::cout << item << " "; });
	return 0;
}
