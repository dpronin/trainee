#include <iostream>
#include <string>

class A {
public:
	A(/* args */) noexcept = default;
	~A() noexcept = default;

	A& operator<<(int i)
	{
		d += i;
		return *this;
	}


	A& operator<<(double d_)
	{
		d += d_;
		return *this;
	}

	A& operator<<(std::string s)
	{
		d *= s.size();
		return *this;
	}

	void operator()()
	{
		std::cout << d << std::endl;
	}
private:
	double d = 0.0;
} a;


int f()
{
	std::cout << "f" << std::endl;
	return 1;
}

double f1()
{
	std::cout << "f1" << std::endl;
	return 2.0;
}

std::string f2()
{
	std::cout << "f2" << std::endl;
	return "Hello, world";
}

int main(int argc, char const *argv[]) {
	a << f() << f1() << f2();
	a();
	return 0;
}
