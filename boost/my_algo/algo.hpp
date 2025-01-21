#ifndef SORTALGO_HPP
#define SORTALGO_HPP

#include <iterator>
#include <vector>
#include <algorithm>
#include <utility>
#include <tuple>

namespace algo {

template<typename RandomIt, typename T>
RandomIt up_bound(RandomIt first, RandomIt last, T const &value) noexcept {
	for (auto it = first + std::distance(first, last) / 2; first < last; it = first + std::distance(first, last) / 2) {
		if (value < *it) last = it;
		else first = it + 1;
	}
	return first;
}

template<typename RandomIt, typename T>
RandomIt low_bound(RandomIt first, RandomIt last, T const &value) noexcept {
	for (auto it = first + std::distance(first, last) / 2; first < last; it = first + std::distance(first, last) / 2) {
		if (value > *it) first = it + 1;
		else last = it;
	}
	return first;
}

template<typename RandomIt, typename T>
RandomIt bin_search(RandomIt first, RandomIt last, T const &value) noexcept {
	auto last_preserved = last;
	for (auto it = first + std::distance(first, last) / 2; first < last; it = first + std::distance(first, last) / 2) {
		if (value > *it) first = it + 1;
		else if (value < *it) last = it;
		else return it;
	}
	return last_preserved;
}

template<typename RandomIt>
void bubble_sort(RandomIt first, RandomIt last) noexcept {
	if (last <= first || first == last - 1) return;

	for (auto i = 0; i < std::distance(first, last) - 1; ++i) {
		bool b_swapped = false;
		for (auto it = first; it < last - i - 1; ++it) {
			if (*it > *(it + 1)) {
				std::iter_swap(it, it + 1);
				b_swapped = true;
			}
		}
		if (!b_swapped) break;
	}
}

template<typename RandomIt>
std::enable_if_t<std::is_integral<typename std::iterator_traits<RandomIt>::value_type>::value>
counting_sort(RandomIt first, RandomIt last) noexcept {
	if (last <= first || first == last - 1) return;

	RandomIt minIt, maxIt;
	std::tie(minIt, maxIt) = std::minmax_element(first, last);
	if (last == minIt || last == maxIt || *maxIt == *minIt) return;

	std::vector<size_t> values(*maxIt - *minIt + 1);
	auto minVal = *minIt;
	std::for_each(first, last, [&values, minVal](auto &&value){
		++values[value - minVal];
	});

	auto it = first;
	for (auto valuesIt = values.begin(); valuesIt != values.end(); ++valuesIt) {
		it = std::fill_n(it, *valuesIt, minVal + std::distance(values.begin(), valuesIt));
	}
}

template<typename ForwardIterator>
void insertion_sort(ForwardIterator first, ForwardIterator last) noexcept {
	if (last <= first || first == last - 1) return;

	for (auto it = first; it < last; ++it) {
		std::rotate(std::upper_bound(first, it, *it), it, it + 1);
	}
}

template<typename BidirectionalIterator>
void quick_sort(BidirectionalIterator first, BidirectionalIterator last) noexcept {
	auto itLeft = first;
	auto itRight = last;

	if (last <= itLeft || itLeft == --itRight) return;

	auto itItem = std::next(first, std::distance(first, last) / 2);

	while (itLeft < itRight)
	{
		while (*itLeft < *itItem) ++itLeft;
		while (*itItem < *itRight) --itRight;
		if (itLeft <= itRight)
		{
			if (itLeft != itRight) std::iter_swap(itLeft, itRight);
			++itLeft;
			--itRight;
		}
	}

	quick_sort(first, std::next(itRight));
	quick_sort(itLeft, last);
}

template<typename ForwardIterator>
void selection_sort(ForwardIterator first, ForwardIterator last) noexcept {
	if (last <= first) return;
	for (auto It = first; It < last - 1; ++It) {
		auto minIt = std::min_element(It, last);
		if (minIt != It) std::iter_swap(It, minIt);
	}
}

} // namespace algo

#endif
