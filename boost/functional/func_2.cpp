#include <iostream>
#include <functional>

#include <vector>

#include <boost/range/algorithm/for_each.hpp>

class A {
public:
	explicit A(int a = 0) : a_(a) {}
	~A() noexcept = default;

	int const& refa() const noexcept { return a_; }

private:
	int a_;
};

void printer(const int &val) {
	std::cout << val << " ";
}

struct gen_s {
	gen_s() noexcept = default;
	~gen_s() noexcept = default;
	A operator()() { ++a; return A{a}; }
	int a = 0;
};

int main(int argc, char const *argv[]) {
	std::vector<A> v;
	std::generate_n(std::back_inserter(v), 10, gen_s());
	boost::range::for_each(v, [](A const &item){ std::cout << item.refa() << " "; });
	std::cout << std::endl;
	std::for_each(v.begin(), v.end(), [] (A const &item) { printer(std::cref(item.refa())); });
	std::cout << std::endl;
	std::for_each(v.begin(), v.end(), std::bind(printer, std::bind(&A::refa, std::placeholders::_1)));
	std::cout << std::endl;
	return 0;
}
