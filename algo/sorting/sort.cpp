#include <cstddef>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <sys/select.h>
#include <vector>

#include "sort_all.hpp"

namespace {

enum class algo {
  bubble,
  heap,
  insertion,
  quick,
  quick_r,
  selection,
  //
  _default_ = bubble,
};

algo algo{algo::_default_};

template <std::ranges::input_range Range> void print(Range const &v) {
  std::ranges::copy(
      v, std::ostream_iterator<typename std::iterator_traits<
             std::ranges::iterator_t<Range>>::value_type>(std::cout, " "));
  std::cout << std::endl;
}

[[noreturn]] void show_help(char const *program) {
  std::string_view const help{
      R"HELP(
--algo           Algorithm to sort: bubble, heap, insertion, quick, quick_r, selection. Default 'bubble'
--help, -h       Show this help page
    )HELP",
  };
  static_assert(algo::_default_ == algo::bubble,
                "change help page to make it consistent");
  std::cout << program << " [params] [item1] [item2] ...\n";
  std::cout << help << std::endl;
  exit(EXIT_SUCCESS);
}

} // namespace

int main(int argc, char const *argv[]) {
  auto v{std::vector<int>{}};

  auto const fargv = argv;
  auto const largv = argv + argc;

  for (auto arg = fargv; largv != arg; ++arg) {
    if ((!strcmp(*arg, "--algo") || !strcmp(*arg, "--algo")) &&
        (arg + 1) != largv) {
      ++arg;
      if (!strcmp(*arg, "bubble")) {
        algo = algo::bubble;
      } else if (!strcmp(*arg, "heap")) {
        algo = algo::heap;
      } else if (!strcmp(*arg, "insertion")) {
        algo = algo::insertion;
      } else if (!strcmp(*arg, "quick")) {
        algo = algo::quick;
      } else if (!strcmp(*arg, "quick_r")) {
        algo = algo::quick_r;
      } else if (!strcmp(*arg, "selection")) {
        algo = algo::selection;
      } else {
        std::cerr << "unknown algorithm specified, use default";
      }
    } else if (!(strcmp(*arg, "--help") && strcmp(*arg, "-h"))) {
      show_help(argv[0]);
      /* must not be here */
      std::abort();
    } else {
      int x;
      std::istringstream{*arg} >> x;
      v.push_back(x);
    }
  }

  std::cout << "unsorted:\n";
  print(v);

  switch (algo) {
  case algo::bubble:
    std::cout << "use bubble sort algorithm" << std::endl;
    bubble_sort(v);
    break;
  case algo::heap:
    std::cout << "use heap sort algorithm" << std::endl;
    heap_sort(v);
    break;
  case algo::insertion:
    std::cout << "use insertion sort algorithm" << std::endl;
    insertion_sort(v);
    break;
  case algo::quick:
    std::cout << "use quick sort algorithm" << std::endl;
    quick_sort(v);
    break;
  case algo::quick_r:
    std::cout << "use recursive quick sort algorithm" << std::endl;
    quick_sort_r(v);
    break;
  case algo::selection:
    std::cout << "use selection sort algorithm" << std::endl;
    selection_sort(v);
    break;
  }

  std::cout << "sorted:\n";
  print(v);

  return 0;
}
