#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/range/iterator_range_core.hpp>

using namespace boost::algorithm;

int func()
{
	std::string s = "Boost C++ k\xfct\xfcphaneleri";
	std::string upper_case1 = ::to_upper_copy(s);
	std::string upper_case2 = to_upper_copy(s, std::locale{"Turkish"});
	std::locale::global(std::locale{"Turkish"});
	std::cout << upper_case1 << '\n';
	std::cout << upper_case2 << '\n';
	return 0;
}

int main(int argc, const char *argv[]) {
	// std::string s = "Boost C++ Libraries";
	// std::cout << to_upper_copy(s) << '\n';
	// to_lower(s);
	// std::cout << s << '\n';
	// func();
	// std::string s = "Boost C++ Libraries";
	// std::cout << erase_first_copy(s, "s") << '\n';
	// std::cout << erase_nth_copy(s, "s", 0) << '\n';
	// std::cout << erase_last_copy(s, "s") << '\n';
	// std::cout << erase_all_copy(s, "s") << '\n';
	// std::cout << erase_tail_copy(s, 9) << '\n';
	// std::cout << erase_head_copy(s, 5) << '\n';

	// std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
	// auto r = boost::make_iterator_range(v.begin(), v.end());
	// std::cout << r << std::endl;

	std::string s = "Boost C++ Libraries";
	// std::cout.setf(std::ios::boolalpha);
	// std::cout << starts_with(s, "Boost") << '\n';
	// std::cout << ends_with(s, "Libraries") << '\n';
	// std::cout << contains(s, "C++") << '\n';
	// std::cout << lexicographical_compare(s, "Boost") << '\n';

	auto r = find_regex(s, boost::regex("\\sL\\w+"));
	std::string s1{r.begin(), r.end()};
	std::cout << trim_left_copy(s1) << std::endl;

	return 0;
}
