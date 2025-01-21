#include <iostream>
#include <string>

#include <boost/format.hpp>

int main(int argc, char const *argv[]) {
	std::cout << boost::format{"%1%.%2%.%3%"} % 12 % 5 % 2014 << '\n';
	std::cout << boost::format{"%+s %s %s"} % 1 % 2 % 1 << '\n';
	return 0;
}
