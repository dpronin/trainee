#include <boost/dll.hpp>

int main(int argc, char const *argv[]) {
	auto f = boost::dll::import<int(int)>("boost_dll/library.so.1.0.0", "func");
	f(2);
	return 0;
}
