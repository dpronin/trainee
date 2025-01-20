#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <vector>

#include <xroost/algo/partition.hpp>

namespace {

template <std::ranges::input_range Range> void print(Range const &v) {
  std::ranges::copy(
      v, std::ostream_iterator<typename std::iterator_traits<
             std::ranges::iterator_t<Range>>::value_type>(std::cout, " "));
  std::cout << std::endl;
}

} // namespace

int main(int argc, char const *argv[]) {
  std::vector<int> v;

  std::copy(std::istream_iterator<int>(std::cin), std::istream_iterator<int>(),
            std::back_inserter(v));

  std::cout << "input array: ";
  print(v);

  auto const pp = xroost::algo::partition(v, [](auto x) { return x < 0; });

  std::cout << "first part of the partitioned array: ";
  print(std::ranges::subrange{v.begin(), pp});

  std::cout << "last part of the partitioned array: ";
  print(std::ranges::subrange{pp, v.end()});

  return 0;
}
