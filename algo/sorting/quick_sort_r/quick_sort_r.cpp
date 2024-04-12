#include <cstddef>

#include <functional>
#include <iterator>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <ranges>

namespace {

template <std::ranges::input_range Range>
void print(Range const &v)
{
    std::ranges::copy(v, std::ostream_iterator<typename std::iterator_traits<std::ranges::iterator_t<Range>>::value_type>(std::cout, " "));
    std::cout << std::endl;
}

template <std::ranges::random_access_range Range, typename Comp = std::ranges::less, typename Proj = std::identity>
requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range> quick_sort_r(Range&& rng, Comp comp = {}, Proj proj = {})
{
    if (auto const dst{std::ranges::size(rng)}; dst > 1) [[likely]] {
        auto first{std::ranges::begin(rng)};
        auto last{std::ranges::end(rng)};
        // the middle item swapped for the first to set the latter to be a pivot item to compare against
        std::iter_swap(first, first + dst / 2);
        // partition the range [first + 1, last) with respect to the first item as the pivot
        auto const mid = std::partition(first + 1, last, [=](auto const &v){ return comp(proj(v), proj(*first)); }) - 1;
        // swap the first item for the last item that in the first part of partitioned range
        // upon this operation the range is devided into [first, mid - 1] < mid <= [mid + 1, last)
        std::iter_swap(first, mid);
        quick_sort_r(std::ranges::subrange{first, mid}, comp, proj);
        quick_sort_r(std::ranges::subrange{mid + 1, last}, comp, proj);
    }
    return std::ranges::end(rng);
}

}

int main(int argc, char const *argv[])
{
    auto v{std::vector<int>(argc - 1)};

    std::transform(argv + 1, argv + argc, v.begin(), [](char const *arg){
        int v;
        std::istringstream{arg} >> v;
        return v;
    });

    std::cout << "unsorted:\n";
    print(v);

    quick_sort_r(v);

    std::cout << "sorted:\n";
    print(v);

    return 0;
}
