#include <iostream>
#include <ostream>
#include <utility>

#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/intrusive_ptr.hpp>

static int ref = 0;

class A {
public:
	explicit A(int a = 0) noexcept : a_(a) {}

	~A() noexcept
	{
		std::cout << "called the destructor" << std::endl;
	}

	friend std::ostream& operator<<(std::ostream &out, A const &a);

	void geta() const {}

private:
	int a_;
};

std::ostream& operator<<(std::ostream &out, A const &a)
{
	out << a.a_;
	return out;
}

void intrusive_ptr_release(A *pA){ --ref; std::cout << "sub ref" << std::endl; if (0 == ref) { std::cout << "delete pA" << std::endl; delete pA; } }
void intrusive_ptr_add_ref(A*){ std::cout << "add ref" << std::endl; ++ref; }

int main(int argc, char const *argv[]) {
	{
		boost::scoped_ptr<A> p;
		p.reset(new A(2));
		std::cout << *p << std::endl;
	}
	{
		boost::scoped_array<A> p{new A[3]};
		for (size_t i = 0; i < 3; ++i) {
			std::cout << p[i] << std::endl;
		}
	}
	boost::shared_ptr<int> g(new int(3), [](int *p){ std::cout << "removing int " << *p << std::endl; delete p; });
	*g = 5;
	g.reset();

	boost::shared_array<float> g1(new float[10]);
	g1[2] = 6.1f;

	{
		boost::intrusive_ptr<A> ai{new A};
		boost::intrusive_ptr<A> ai2 = ai;
	}

	return 0;
}
