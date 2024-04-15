#include <cstddef>

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <vector>

template <std::ranges::random_access_range Range,
          typename Comp = std::ranges::less, typename Proj = std::identity>
  requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range>
bubble_sort(Range &&rng, Comp comp = {}, Proj proj = {}) {
  auto first{std::ranges::begin(rng)};
  auto last{std::ranges::end(rng)};

  for (auto j_last{last - 1}; first < j_last; --j_last) {
    bool swapped{false};

    for (auto j_first{first}; j_first < j_last; ++j_first) {
      if (comp(proj(j_first[1]), proj(j_first[0]))) {
        std::iter_swap(j_first + 1, j_first);
        swapped = true;
      }
    }

    if (!swapped)
      break;
  }

  return std::ranges::end(rng);
}
