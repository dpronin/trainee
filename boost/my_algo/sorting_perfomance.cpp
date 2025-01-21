#include <random>
#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/timer/timer.hpp>
#include <boost/lexical_cast.hpp>
#include "algo.hpp"

std::string FormatBytes(size_t bytes)
{
	std::array<std::string, 5> const sizes = { "B", "KB", "MB", "GB", "TB" };

	int i;
	double dblByte = bytes;
	for (i = 0; i < 5 && bytes >= 1024; i++, bytes /= 1024) {
		dblByte = bytes / 1024.0;
	}

	return std::to_string(dblByte) + sizes[i];
}


int main(int argc, char const *argv[]) {
	size_t qty = boost::lexical_cast<size_t>(argv[1]);
	std::cerr << "qty: " << qty << ", size: " << FormatBytes(qty * sizeof(float)) << std::endl;

	std::vector<float> v(qty);

	std::random_device d;
	std::default_random_engine e{d()};

	std::generate(v.begin(), v.end(), e);

	std::vector<float> v_test(v);
	std::cerr << "generic:" << std::endl;
	{
		boost::timer::cpu_timer tm;
		// algo::bubble_sort(v.begin(), v.end());
		std::sort(v_test.begin(), v_test.end());
		std::cerr << tm.format();
	}

	v_test = v;
	std::cerr << "with lmbd:" << std::endl;
	{
		boost::timer::cpu_timer tm2;
		// algo::selection_sort(v.begin(), v.end());
		std::sort(v_test.begin(), v_test.end(), [](float a, float b){ return a < b; });
		std::cerr << tm2.format();
	}

	return 0;
}
