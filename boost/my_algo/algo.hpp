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

template <typename BidirectionalIterator>
void quick_sort(BidirectionalIterator first,
                BidirectionalIterator last) noexcept {
  auto itLeft = first;
  auto itRight = last;

  if (last <= itLeft || itLeft == --itRight)
    return;

  auto itItem = std::next(first, std::distance(first, last) / 2);

  while (itLeft < itRight) {
    while (*itLeft < *itItem)
      ++itLeft;
    while (*itItem < *itRight)
      --itRight;
    if (itLeft <= itRight) {
      if (itLeft != itRight)
        std::iter_swap(itLeft, itRight);
      ++itLeft;
      --itRight;
    }
  }

  quick_sort(first, std::next(itRight));
  quick_sort(itLeft, last);
}

} // namespace algo

#endif
