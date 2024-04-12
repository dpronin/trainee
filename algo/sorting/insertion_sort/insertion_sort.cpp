#include <cstddef>

#include <functional>
#include <iterator>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <ranges>

template <std::ranges::input_range Range>
void print(Range const &v)
{
    std::ranges::copy(v, std::ostream_iterator<typename std::iterator_traits<std::ranges::iterator_t<Range>>::value_type>(std::cout, " "));
    std::cout << std::endl;
}

template <std::ranges::random_access_range Range, typename Comp = std::ranges::less, typename Proj = std::identity>
requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range> insertion_sort(Range&& rng, Comp comp = {}, Proj proj = {})
{
    auto first{std::ranges::begin(rng)};
    auto last{std::ranges::end(rng)};
    for (auto i{first + 1}; i < last; ++i)
        std::rotate(std::ranges::upper_bound(std::ranges::subrange{first, i}, *i, comp, proj), i, i + 1);
    return std::ranges::end(rng);
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

    insertion_sort(v);

    std::cout << "sorted:\n";
    print(v);

    return 0;
}
