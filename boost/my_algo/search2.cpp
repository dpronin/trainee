#include <vector>
#include <unordered_set>
#include <iostream>
#include <utility>

int main(int argc, char const *argv[]) {
	std::vector<int> A = {1, 2, 3, 4, 5, 6};
	std::vector<int> B = {7, 8, 9, 10, 11, 12};
	int const target = 10;

	std::vector<std::pair<int, int>> result;

	std::unordered_set<int> m {B.begin(), B.end()};
	for (auto a : A) {
		int const b = target - a;
		auto it = m.find(b);
		if (it != m.end()) result.emplace_back(a, b);
	}

	for (auto const &item : result) {
		std::cout << "{ " << item.first << ", " << item.second << " }" << std::endl;
	}
	return 0;
}
