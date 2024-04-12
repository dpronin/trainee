#include <functional>
#include <iostream>
#include <ranges>
#include <vector>

int main(int argc, char const *argv[]) {
  auto temperatures = {28, 25, -8, -3, 15, 21, -1};
  auto minus = [](int i) { return i <= 0; };
  auto toFahrenheit = [](int i) { return (i * (9. / 5)) + 32; };
  for (auto t : temperatures | std::views::filter(std::not_fn(minus)) |
                    std::views::transform(toFahrenheit)) {
    std::cout << t << ' ';
  }
  std::cout << std::endl;
  return 0;
}
