#include <string>
#include <iostream>

#include <boost/circular_buffer.hpp>
#include <boost/core/addressof.hpp>

class A {
public:
	A(std::string const &name) noexcept : name_(name)
	{
		std::cout << "called constructor of " << boost::addressof(*this) << std::endl;
	}

	~A()
	{
		std::cout << "called destructor of " << boost::addressof(*this) << std::endl;
	}

	std::string const& refname() const { return name_; }

private:
	std::string name_;
};

int main(int argc, char const *argv[]) {
	boost::circular_buffer<A> a(3);
	a.push_back(A("lion"));
	a.push_back(A("bird"));
	a.push_back(A("rabbit"));
	a.push_back(A("wolf"));

	for (auto&& item : a) {
		std::cout << item.refname() << " ";
	}
	std::cout << std::endl;
	return 0;
}
