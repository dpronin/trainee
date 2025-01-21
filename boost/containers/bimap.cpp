#include <boost/bimap/bimap.hpp>

#include <string>
#include <iostream>

int main(int argc, char const *argv[]) {
	using animals_type = boost::bimaps::bimap<std::string, int>;
	animals_type animals;

	animals.insert({"lion", 4});
	animals.insert({"tiger", 5});
	animals.insert({"bird", 6});
	animals.insert({"port", 7});

	auto &index = animals.right;
	auto it = index.find(4);
	if (it != index.end()) {
		std::cout << it->second << std::endl;
	}

	return 0;
}
