#include <vector>
#include <unordered_map>
#include <iostream>
#include <utility>
#include <tuple>

int main(int argc, char const *argv[]) {
	std::vector<int> A = {1, 2, 3, 4, 5, 6};
	std::vector<int> B = {7, 8, 9, 10, 11, 12};
	std::vector<int> C = {0, 2, -2, -6, 9, 4};
	auto constexpr target = 10;

	std::vector<std::tuple<int, int, int>> result;

	std::unordered_multimap<int, int> m;
	for (auto a : A) {
		for (auto b : B) m.emplace(a + b, a);
	}

	for (auto c : C) {
		auto pair = m.equal_range(target - c);
		for (auto it = pair.first; it != pair.second; ++it) {
			result.emplace_back(it->second, it->first - it->second, c);
		}
	}

	for (auto const &item : result) {
		std::cout << "{ " << std::get<0>(item) << ", " << std::get<1>(item) << ", " << std::get<2>(item) << " }";
		std::cout << std::endl;
	}
	return 0;
}
