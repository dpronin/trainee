/**
 * Example of running:
 *
 * $ echo -1 2 9 10 0 -7 19 -9 5 5 40 | ./insertion_sort_one_by_one
 *
 * Author: Denis Pronin <dannftk@yandex.ru>
 */

#include <cstddef>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <vector>

namespace {

template <std::ranges::input_range Range> void print(Range const &v) {
  std::ranges::copy(
      v, std::ostream_iterator<typename std::iterator_traits<
             std::ranges::iterator_t<Range>>::value_type>(std::cout, " "));
}

[[noreturn]] void show_help(char const *program) {
  std::string_view const help{
      R"HELP(
--help, -h       Show this help page
    )HELP",
  };
  std::cout << program << " [params] ...\n";
  std::cout << help << std::endl;
  exit(EXIT_SUCCESS);
}

} // namespace

int main(int argc, char const *argv[]) {
  for (auto const *arg : std::ranges::subrange{argv, argv + argc}) {
    if (!(strcmp(arg, "--help") && strcmp(arg, "-h"))) {
      show_help(argv[0]);
      /* must not be here */
      std::abort();
    }
  }

  std::vector<int> v{};

  auto const cin_range{
      std::ranges::subrange{std::istream_iterator<int>(std::cin),
                            std::istream_iterator<int>()},
  };

  for (auto const in_value : cin_range)
    v.insert(std::ranges::upper_bound(v, in_value), in_value);

  std::ranges::copy(v, std::ostream_iterator<int>(std::cout, " "));

  std::cout << std::endl;

  return 0;
}
