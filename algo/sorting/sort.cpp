/**
 * Example of running:
 *
 * $ echo -1 2 9 10 0 -7 19 -9 5 5 40 | ./sort --algo heap
 *
 * Author: Denis Pronin <dannftk@yandex.ru>
 */

#include <cstddef>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string_view>
#include <vector>

#include <xroost/algo/sorting/sort_all.hpp>

namespace {

enum class algo {
  bubble,
  heap,
  insertion,
  quick,
  quick_r,
  selection,
  counting,
  //
  _default_ = bubble,
};

algo algo{algo::_default_};

template <std::ranges::input_range Range> void print(Range const &v) {
  std::ranges::copy(
      v, std::ostream_iterator<typename std::iterator_traits<
             std::ranges::iterator_t<Range>>::value_type>(std::cout, " "));
}

[[noreturn]] void show_help(char const *program) {
  std::string_view const help{
      R"HELP(
--algo           Algorithm to sort: bubble, heap, insertion, quick, quick_r, selection, counting. Default 'bubble'
--help, -h       Show this help page
    )HELP",
  };
  static_assert(algo::_default_ == algo::bubble,
                "change help page to make it consistent");
  std::cout << program << " [params] ...\n";
  std::cout << help << std::endl;
  exit(EXIT_SUCCESS);
}

} // namespace

int main(int argc, char const *argv[]) {
  auto v{std::vector<int>{}};

  auto const fargv = argv;
  auto const largv = argv + argc;

  for (auto const *arg{fargv}; largv != arg; ++arg) {
    if (auto sarg{std::string_view{*arg}};
        "--argo" == sarg && (arg + 1) != largv) {
      ++arg;
      if ("bubble" == sarg) {
        algo = algo::bubble;
      } else if ("heap" == sarg) {
        algo = algo::heap;
      } else if ("insertion" == sarg) {
        algo = algo::insertion;
      } else if ("quick" == sarg) {
        algo = algo::quick;
      } else if ("quick_r" == sarg) {
        algo = algo::quick_r;
      } else if ("selection" == sarg) {
        algo = algo::selection;
      } else if ("counting" == sarg) {
        algo = algo::counting;
      } else {
        std::cerr << "unknown algorithm specified, use default";
      }
    } else if (!(strcmp(*arg, "--help") && strcmp(*arg, "-h"))) {
      show_help(argv[0]);
      /* must not be here */
      std::abort();
    }
  }

  std::copy(std::istream_iterator<int>(std::cin), std::istream_iterator<int>(),
            std::back_inserter(v));

  std::cout << "unsorted:\n";
  print(v);
  std::cout << std::endl;

  switch (algo) {
  case algo::bubble:
    std::cout << "use bubble sort algorithm" << std::endl;
    xroost::algo::bubble_sort(v);
    break;
  case algo::heap:
    std::cout << "use heap sort algorithm" << std::endl;
    xroost::algo::heap_sort(v);
    break;
  case algo::insertion:
    std::cout << "use insertion sort algorithm" << std::endl;
    xroost::algo::insertion_sort(v);
    break;
  case algo::quick:
    std::cout << "use quick sort algorithm" << std::endl;
    xroost::algo::quick_sort(v);
    break;
  case algo::quick_r:
    std::cout << "use recursive quick sort algorithm" << std::endl;
    xroost::algo::quick_sort_r(v);
    break;
  case algo::selection:
    std::cout << "use selection sort algorithm" << std::endl;
    xroost::algo::selection_sort(v);
    break;
  case algo::counting:
    std::cout << "use counting sort algorithm" << std::endl;
    xroost::algo::counting_sort(v);
    break;
  }

  std::cout << "sorted:\n";
  print(v);
  std::cout << std::endl;

  return 0;
}
