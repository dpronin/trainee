#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <vector>

namespace {

template <std::ranges::input_range Range> void print(Range const &v) {
  std::ranges::copy(
      v, std::ostream_iterator<typename std::iterator_traits<
             std::ranges::iterator_t<Range>>::value_type>(std::cout, " "));
  std::cout << std::endl;
}

template <std::permutable Iterator, std::sentinel_for<Iterator> Sentinel,
          typename Proj = std::identity,
          std::indirect_unary_predicate<std::projected<Iterator, Proj>> Pred>
Iterator partition(Iterator first, Sentinel last, Pred pred, Proj proj = {}) {
  first =
      std::ranges::find_if_not(std::ranges::subrange{first, last}, pred, proj);
  if (first == last)
    return first;

  for (auto n_first = std::next(first); n_first != last; ++n_first) {
    if (pred(proj(*n_first)))
      std::iter_swap(first++, n_first);
  }

  return first;
}

template <std::ranges::forward_range Range, typename Proj = std::identity,
          std::indirect_unary_predicate<
              std::projected<std::ranges::iterator_t<Range>, Proj>>
              Pred = std::ranges::less>
std::ranges::iterator_t<Range> partition(Range &&range, Pred pred = {},
                                         Proj proj = {}) {
  return partition(std::ranges::begin(range), std::ranges::end(range), pred,
                   proj);
}

} // namespace

int main(int argc, char const *argv[]) {
  auto v{std::vector<int>(argc - 1)};

  std::transform(argv + 1, argv + argc, v.begin(), [](char const *arg) {
    int v;
    std::istringstream{arg} >> v;
    return v;
  });

  std::cout << "input array: ";
  print(v);

  auto const pp = partition(v, [](auto x) { return x < 0; });

  std::cout << "first part partitioned array: ";
  print(std::ranges::subrange{v.begin(), pp});

  std::cout << "last part partitioned array: ";
  print(std::ranges::subrange{pp, v.end()});

  return 0;
}
