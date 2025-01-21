#include <utility>
#include <iostream>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/container/stable_vector.hpp>

class A {
public:
	A() noexcept = default;
	~A() noexcept
	{
		std::cout << "called destructor" << std::endl;
	}

	int geta() const { return a; }
private:
	int a;
};

int main(int argc, char const *argv[]) {
	// boost::ptr_vector<A> pv;
	// boost::container::stable_vector<A> sv;

	// pv.push_back(new A());
	// pv.push_back(new A());
	// pv.push_back(new A());
	// pv.push_back(new A());

	// A &item = pv[0];
	// pv.erase(pv.begin() + 1);
	// std::cout << item.geta() << std::endl;

	std::cout << typeid(*std::declval<boost::ptr_vector<A>>().begin()).name() << std::endl;
	std::cout << typeid(*std::declval<boost::container::vector<A>>().begin()).name() << std::endl;

	std::cout << typeid(std::declval<boost::ptr_vector<A>>()[0]).name() << std::endl;
	std::cout << typeid(std::declval<boost::container::vector<A>>()[0]).name() << std::endl;

	std::cout << typeid(int*).name() << std::endl;

	// sv.push_back(A());
	// sv.push_back(A());
	// sv.push_back(A());
	// sv.push_back(A());

	// A &item2 = sv[0];
	// sv.erase(sv.begin() + 1);
	// std::cout << item2.geta() << std::endl;

	// pv.clear();
	// sv.clear();
	// sv.erase(sv.begin());

	return 0;
}
