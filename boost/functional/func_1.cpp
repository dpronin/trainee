#include <iostream>

#include <boost/bind.hpp>
#include <boost/function.hpp>

void foo(int value, int value2) {
	std::cout << "sum " << value << " with " << value2 << " is " << value + value2 << std::endl;
}

int main(int argc, char const *argv[]) {
	boost::function<void(int)> f = boost::bind(foo, 4, _1);
	f(6);
	return 0;
}
