#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using namespace boost::asio;
using namespace std::literals;

int main()
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::seconds{3}};
  timer1.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  steady_timer timer2{ioservice, std::chrono::seconds{4}};
  timer2.async_wait([](const boost::system::error_code &ec)
    { std::cout << "4 sec\n"; });

  std::cout << "sleep for 3 sec" << std::endl;
  std::this_thread::sleep_for(3s);
  std::cout << "start timers..." << std::endl;
  ioservice.run();
  std::cout << "timers canceled.Done" << std::endl;
  return 0;
}
