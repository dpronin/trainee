#include <string>
#include <tuple>
#include <utility>

#include <iostream>

#include <boost/ref.hpp>

class B {
public:
	B() noexcept = default;
	~B()
	{
		std::cout << "B dtor" << std::endl;
	}

	B(const B &that)
	{
		std::cout << "copy" << std::endl;
	}

	B(B &&that)
	{
		std::cout << "move" << std::endl;
	}

	B& operator=(const B &that)
	{
		std::cout << "copy as" << std::endl;
		return *this;
	}

	B& operator=(B &&that)
	{
		std::cout << "move as" << std::endl;
		return *this;
	}

	void setStr(std::string const &s) { s_ = s; }
	std::string const& getStr() const { return s_; }

private:
	std::string s_;
};

class A {
public:
	A() noexcept = default;
	~A()
	{
		std::cout << "A dtor" << std::endl;
	}

	void setStr(std::string const &s) { b_.setStr(s); }
	B getB() { return std::move(b_); }
private:
	B b_;
};

int main(int argc, char const *argv[]) {
	// std::tuple<std::string, std::string> t{"hello", "world"};
	// std::string s = std::get<0>(t);
	// std::string &s1 = std::get<1>(t);
	// std::cout << s << std::endl;

	A a;
	a.setStr("Hello");
	{
		B const &b = a.getB();
		std::cout << "end of scope " << b.getStr() << std::endl;
	}

	std::string str = "Hello, world";

	std::string &str2 = boost::ref(str);
	std::cout << str2 << std::endl;

	return 0;
}
