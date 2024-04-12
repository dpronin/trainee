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
constexpr std::ranges::borrowed_iterator_t<Range> make_heap(Range&& rng, Comp comp = {}, Proj proj = {})
{
    for (auto i = 1z; i < std::ranges::size(rng); ++i) {
        for (auto k = i, j = k % 2 ? k / 2 : k / 2 - 1;
             !(j < 0);
             k = j, j = k % 2 ? k / 2 : k / 2 - 1) {
            if (comp(proj(rng[j]), proj(rng[k]))) {
                using std::swap;
                swap(rng[j], rng[k]);
            } else {
                break;
            }
        }
    }
    return std::ranges::end(rng);
}

template <std::ranges::random_access_range Range, typename Comp = std::ranges::less, typename Proj = std::identity>
requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range> pop_heap(Range&& rng, Comp comp = {}, Proj proj = {})
{
    if (std::ranges::size(rng) < 2)
        return std::ranges::end(rng);

    using std::swap;
    swap(rng[0], rng[std::ranges::size(rng) - 1]);

    auto wrng{std::ranges::subrange{std::ranges::begin(rng), std::ranges::end(rng) - 1}};

    auto i{0z};

    for (; 2 * (i + 1) < std::ranges::size(wrng); ) {
        if (comp(proj(wrng[i]), proj(wrng[2 * i + 1]))) {
            auto const j = comp(proj(wrng[2 * i + 1]), proj(wrng[2 * (i + 1)])) ? 2 * (i + 1) : 2 * i + 1;
            swap(wrng[i], wrng[j]);
            i = j;
        } else if (comp(proj(wrng[i]), proj(wrng[2 * (i + 1)]))) {
            auto const j = 2 * (i + 1);
            swap(wrng[i], wrng[j]);
            i = j;
        } else {
            break;
        }
    }

    if (2 * i + 1 < std::ranges::size(wrng) && comp(proj(wrng[i]), proj(wrng[2 * i + 1])))
        swap(wrng[i], wrng[2 * i + 1]);

    return std::ranges::end(rng);
}

template <std::ranges::random_access_range Range, typename Comp = std::ranges::less, typename Proj = std::identity>
requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range> sort_heap(Range&& rng, Comp comp = {}, Proj proj = {})
{
    auto first = std::ranges::begin(rng);
    auto last = std::ranges::end(rng);
    for (; first != last; --last)
        pop_heap(std::ranges::subrange{first, last}, comp, proj);
    return std::ranges::end(rng);
}

template <std::ranges::random_access_range Range, typename Comp = std::ranges::less, typename Proj = std::identity>
requires std::sortable<std::ranges::iterator_t<Range>, Comp, Proj>
constexpr std::ranges::borrowed_iterator_t<Range> heap_sort(Range&& rng, Comp comp = {}, Proj proj = {})
{
    make_heap(rng, comp, proj);
    sort_heap(rng, comp, proj);
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

    heap_sort(v);

    std::cout << "sorted:\n";
    print(v);

    return 0;
}
