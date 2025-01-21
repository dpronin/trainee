#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <utility>
#include <tuple>

int main(int argc, char const *argv[]) {
	std::vector<int> nums = {0, 0, 0};
	auto constexpr target = 0;

	std::set<std::vector<int>> result;

	std::unordered_multimap<int, std::pair<int, int>> m;
	for (int i = 0; i < nums.size() - 1; ++i) {
		for (int j = i + 1; j < nums.size(); ++j) m.emplace(nums[i] + nums[j], std::make_pair(i, j));
	}

	for (int i = 0; i < nums.size(); ++i) {
		auto pair = m.equal_range(target - nums[i]);
		for (auto it = pair.first; it != pair.second; ++it) {
			if (i > it->second.first && i > it->second.second) {
				auto s = std::multiset<int>{nums[it->second.first], nums[it->second.second], nums[i]};
				result.emplace(s.begin(), s.end());
			}
		}
	}

	std::vector<std::vector<int>> res{std::make_move_iterator(result.begin()), std::make_move_iterator(result.end())};

	for (auto const &item : res) {
		std::cout << "{ " << item[0] << ", " << item[1] << ", " << item[2] << " }";
		std::cout << std::endl;
	}
	return 0;
}
