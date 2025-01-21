#include <string>
#include <iostream>

#include <boost/cast.hpp>
#include <boost/type_index.hpp>
#include <boost/type_traits/has_post_increment.hpp>

template<typename T>
std::enable_if_t<boost::has_post_increment<T>::value> print_has_post_increment()
{
	std::cout << boost::typeindex::type_id<T>().pretty_name() << " has post increment" << std::endl;
}

template<typename T>
std::enable_if_t<!boost::has_post_increment<T>::value> print_has_post_increment()
{
	std::cout << boost::typeindex::type_id<T>().pretty_name() << " does not have post increment" << std::endl;
}

int main(int argc, const char *argv[])
{
	print_has_post_increment<int>();
	print_has_post_increment<long>();
	print_has_post_increment<std::string>();
	return 0;
}
