#include <boost/intrusive/list.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <iostream>

struct Compare;

class A
	: public boost::intrusive::list_base_hook<>
	, public boost::intrusive::set_base_hook<> {
public:
	friend Compare;
	// using is_transparent = void;

	A(int a, float b) : a_(a), b_(b) {}
	~A() noexcept = default;

	bool operator <(A const &that) const
	{
		return boost::make_tuple(a_, b_) < boost::make_tuple(that.a_, that.b_);
	}

	int getInt() const { return a_; }
	float getFloat() const { return b_; }

private:
	int a_;
	float b_;
};

struct Compare {
	Compare() noexcept = default;
	~Compare() noexcept = default;

	bool operator()(std::pair<int, float> key, A const &that) const
	{
		return boost::make_tuple(key.first, key.second) < boost::make_tuple(that.a_, that.b_);;
	}

	bool operator()(A const &that, std::pair<int, float> key) const
	{
		return boost::make_tuple(key.first, key.second) > boost::make_tuple(that.a_, that.b_);;
	}
};

int main(int argc, char const *argv[]) {
	boost::intrusive::list<A> lst;

	auto *pA = new A(1, 2.0);
	auto *pA1 = new A(2, 1.0);
	auto *pA2 = new A(3, 4.0);
	auto *pA3 = new A(4, 3.0);

	lst.push_back(*pA);
	lst.push_back(*pA1);
	lst.push_back(*pA2);
	lst.push_back(*pA3);

	boost::intrusive::set<A> st;

	st.insert(*pA);
	st.insert(*pA1);
	st.insert(*pA2);
	st.insert(*pA3);

	auto it = st.find(std::make_pair(3, 4.0), Compare());
	if (it != st.end())
	{
		std::cout << it->getInt() << " " << it->getFloat() << std::endl;
	}

	return 0;
}
