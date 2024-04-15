#include <cstddef>

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <ranges>
#include <vector>

template <std::ranges::random_access_range Range,
          typename Comp = std::ranges::less, typename Proj = std::identity>
  requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range>
insertion_sort(Range &&rng, Comp comp = {}, Proj proj = {}) {
  auto first{std::ranges::begin(rng)};
  auto last{std::ranges::end(rng)};
  for (auto i{first + 1}; i < last; ++i)
    std::rotate(std::ranges::upper_bound(std::ranges::subrange{first, i}, *i,
                                         comp, proj),
                i, i + 1);
  return std::ranges::end(rng);
}
