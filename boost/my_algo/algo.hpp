#ifndef SORTALGO_HPP
#define SORTALGO_HPP

#include <algorithm>
#include <iterator>
#include <tuple>
#include <utility>
#include <vector>

namespace algo {

template <typename RandomIt, typename T>
RandomIt up_bound(RandomIt first, RandomIt last, T const &value) noexcept {
  for (auto it = first + std::distance(first, last) / 2; first < last;
       it = first + std::distance(first, last) / 2) {
    if (value < *it)
      last = it;
    else
      first = it + 1;
  }
  return first;
}

template <typename RandomIt, typename T>
RandomIt low_bound(RandomIt first, RandomIt last, T const &value) noexcept {
  for (auto it = first + std::distance(first, last) / 2; first < last;
       it = first + std::distance(first, last) / 2) {
    if (value > *it)
      first = it + 1;
    else
      last = it;
  }
  return first;
}

template <typename RandomIt, typename T>
RandomIt bin_search(RandomIt first, RandomIt last, T const &value) noexcept {
  auto last_preserved = last;
  for (auto it = first + std::distance(first, last) / 2; first < last;
       it = first + std::distance(first, last) / 2) {
    if (value > *it)
      first = it + 1;
    else if (value < *it)
      last = it;
    else
      return it;
  }
  return last_preserved;
}

template <typename ForwardIterator>
void insertion_sort(ForwardIterator first, ForwardIterator last) noexcept {
  if (last <= first || first == last - 1)
    return;

  for (auto it = first; it < last; ++it) {
    std::rotate(std::upper_bound(first, it, *it), it, it + 1);
  }
}

} // namespace algo

#endif
