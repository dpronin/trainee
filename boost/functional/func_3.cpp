#include <string>
#include <iostream>

#include <functional>

std::function<void()> foo() {
	auto f = []{ std::cout << "hello, world" << std::endl; };
	return f;
}

void bar(int a, int b) {
	std::cout << a << " " << b << std::endl;
}

void fff(int a, int b)
{
	std::cout << a + b << std::endl;
}

int main(int argc, char const *argv[]) {
	auto f = foo();
	int a = 2, b = 3;
	bar(a, b);
	f();

	std::function<void(int)> f2 = std::bind(fff, 5, std::placeholders::_1);
	f2(3);

	return 0;
}
