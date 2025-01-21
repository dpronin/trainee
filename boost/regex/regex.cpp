#include <iostream>
#include <string>

#include <boost/regex.hpp>

int main(int argc, char const *argv[]) {
	// std::string s = "Boost Libraries";
	// boost::regex expr{"(\\w+)\\s(\\w+)"};
	// boost::smatch what;
	// if (boost::regex_search(s, what, expr))
	// {
	// 	std::cout << what[0] << '\n';
	// 	std::cout << what[1] << "_" << what[2] << '\n';
	// }
	// std::string f1 = what[1];
	// std::cout << f1 << std::endl;

	std::string s = "Boost Libraries";
	boost::regex expr{"\\w+"};
	boost::regex_token_iterator<std::string::iterator> it{s.begin(), s.end(), expr};
	boost::regex_token_iterator<std::string::iterator> end;
	while (it != end) std::cout << *it++ << '\n';

	return 0;
}
