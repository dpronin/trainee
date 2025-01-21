#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

int main()
{
	boost::uuids::random_generator gen;
	boost::uuids::uuid id = gen();
	std::cout << id << '\n';
}
