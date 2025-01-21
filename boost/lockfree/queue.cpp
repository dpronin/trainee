#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using namespace std::literals;

template<typename TYPE>
// using lockfree_queue_t = boost::lockfree::spsc_queue<int>;
using lockfree_queue_t = boost::lockfree::queue<int>;

int main(int argc, char const *argv[]) {
	lockfree_queue_t<int> q(100);

	std::vector<std::thread> threads;
	threads.reserve(10);

	threads.emplace_back([&]() noexcept {
		int i = 0;
		while (q.push(i))
		{
			std::ostringstream os;
			os << "produced request " << i << '\n';
			std::cout << os.str();
			++i;
			std::this_thread::sleep_for(10ms);
		}
		std::cout << "producer thread is done\n";
	});

	std::this_thread::sleep_for(200ms);

	for (size_t thi = 0; thi < 9; ++thi) {
		threads.emplace_back([&]() noexcept {
			int i = 0;
			while (q.pop(i))
			{
				std::ostringstream os;
				os << "processing request " << i << '\n';
				std::cout << os.str();
				std::this_thread::sleep_for(120ms);
			}
			std::cout << "consumer thread is done\n";
		});
	}

	for (auto&& item : threads) {
		item.join();
	}

	return 0;
}
