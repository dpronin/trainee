#include <string>
#include <iostream>

#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#include <boost/core/addressof.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

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
	boost::ptr_vector<A> vec;
	vec.push_back(new A("hello"));
	return 0;
}
