#include <boost/any.hpp>
#include <iostream>

int main()
{
	boost::any a = 1;
	auto b = boost::any_cast<int>(a);
	try {
		auto *pC = boost::any_cast<bool>(&a);
		if (nullptr == pC) std::cerr << "something wrong when casting" << std::endl;
		auto d = boost::any_cast<std::string>(a);
	}
	catch (boost::bad_any_cast const& ex) {
		std::cerr << ex.what() << std::endl;
	}
	std::cout << b << std::endl;
}
