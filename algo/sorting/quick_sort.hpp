#include <cstddef>

#include <algorithm>
#include <functional>
#include <iterator>
#include <ranges>
#include <stack>

template <std::ranges::random_access_range Range,
          typename Comp = std::ranges::less, typename Proj = std::identity>
  requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range>
quick_sort(Range &&rng, Comp comp = {}, Proj proj = {}) {
  if (std::ranges::size(rng) > 1) [[likely]] {
    std::stack<decltype(std::ranges::subrange{rng})> s;
    s.push(rng);
    while (!s.empty()) {
      auto const [first, last] = s.top();
      s.pop();
      // the middle item swapped for the first to set the latter to be a pivot
      // item to compare against
      std::iter_swap(first, first + (last - first) / 2);
      // partition the range [first + 1, last) with respect to the first item as
      // the pivot
      auto mid = std::partition(first + 1, last,
                                [=](auto const &v) {
                                  return comp(proj(v), proj(*first));
                                }) -
                 1;
      // swap the first item for the last item that in the first part of
      // partitioned range upon this operation the range is devided into [first,
      // mid - 1] < mid <= [mid + 1, last)
      std::iter_swap(first, mid);
      if (mid - first > 1) [[likely]]
        s.push({first, mid});
      ++mid;
      if (last - mid > 1) [[likely]]
        s.push({mid, last});
    }
  }
  return std::ranges::end(rng);
}
