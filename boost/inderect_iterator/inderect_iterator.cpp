#include <vector>
#include <iostream>
#include <algorithm>

#include <boost/iterator/indirect_iterator.hpp>

// template<typename IteratorType>
// decltype(auto) make_indirect_iterator(IteratorType iterator)
// {
// 	return boost::indirect_iterator<IteratorType>(iterator);
// }

int main(int argc, char const *argv[]) {
	std::vector<std::unique_ptr<int>> v;
	v.push_back(std::make_unique<int>(1));
	v.push_back(std::make_unique<int>(2));
	v.push_back(std::make_unique<int>(3));
	v.push_back(std::make_unique<int>(4));
	std::copy(boost::make_indirect_iterator(v.begin()),
		boost::make_indirect_iterator(v.end()),
		std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;
	std::remove_if(boost::make_indirect_iterator(v.begin()),
		boost::make_indirect_iterator(v.end()),
		[](int value){ return value % 2; });
	std::copy(boost::make_indirect_iterator(v.begin()),
		boost::make_indirect_iterator(v.end()),
		std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;
	return 0;
}
