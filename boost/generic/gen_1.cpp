#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

struct A {
	typedef int (A::*ptr_to_mem);
	A() noexcept = default;
	~A() noexcept = default;

	ptr_to_mem getPtr() { return &A::a_; }

	int geta() const { return a_; }

private:
	int a_ = 0;
};

int main(int argc, char const *argv[]) {
	A a;
	std::cout << a.geta() << std::endl;

	(a.*a.getPtr()) = 2;

	std::cout << a.geta() << std::endl;

	std::vector<int> v = { 1, 2, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 7 };
	// std::reverse(v.begin(), v.end());
	std::copy_backward(v.rend() - 3, v.rend(), v.end());
	// auto it = std::lower_bound(v.rbegin(), v.rend(), 4);
	// auto it2 = std::upper_bound(v.rbegin(), v.rend(), 5);
	// if (it != v.rend() && it2 != v.rend()) {
	// 	// auto it3 = std::rotate(v.rbegin(), v.rbegin() + 1, v.rend());
		for (auto&& item : v) {
			std::cout << item << " ";
		}
		// std::cout << "!!! " << *it << " !!! " << *it2 << std::endl;
		// std::cout << std::endl;
	// }

	return 0;
}
