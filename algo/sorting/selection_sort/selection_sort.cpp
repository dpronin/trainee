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
constexpr std::ranges::borrowed_iterator_t<Range> selection_sort(Range&& rng, Comp comp = {}, Proj proj = {})
{
    auto first{std::ranges::begin(rng)};
    auto last{std::ranges::end(rng)};

    for (; first < last - 1; ++first)
        std::iter_swap(first, std::ranges::min_element(std::ranges::subrange{first, last}, comp, proj));

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

    selection_sort(v);

    std::cout << "sorted:\n";
    print(v);

    return 0;
}
