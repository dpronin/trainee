#include <iostream>

int main(int argc, char const *argv[]) {
	auto f = [](int a) { return [a](int b){ return a + b; }; };
	auto f1 = f(3);

	std::cout << std::boolalpha << (f(3)(3) == f1(3)) << std::endl;
	std::cout << std::boolalpha << (f(3)(0) == f1(0)) << std::endl;
	std::cout << std::boolalpha << (f(3)(1) == f1(1)) << std::endl;
	std::cout << std::boolalpha << (f(3)(2) == f1(2)) << std::endl;

	return 0;
}
