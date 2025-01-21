#include <string>
#include <iostream>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/core/addressof.hpp>

namespace bi = boost::intrusive;
using hook_t = bi::list_member_hook<bi::link_mode<bi::auto_unlink>>;

class A {
public:
	A(std::string const &name) noexcept : name_(name) {};
	~A()
	{
		std::cout << "called destructor of " << boost::addressof(*this) << std::endl;
	}

	std::string const& refname() const { return name_; }

	hook_t hook;
private:
	std::string name_;
};

int main(int argc, char const *argv[]) {
	A a("lion");
	bi::list<A, bi::constant_time_size<false>, bi::member_hook<A, hook_t, &A::hook>> lst;
	lst.push_back(a);
	auto *pB = new A("bird");
	std::cout << "created an object " << boost::addressof(*pB) << std::endl;
	lst.push_back(*pB);
	std::cout << lst.size() << std::endl;
	delete pB;
	std::cout << lst.size() << std::endl;
	return 0;
}
