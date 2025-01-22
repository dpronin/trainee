#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/core/lightweight_test.hpp>

#include <xroost/algo/sorting/insertion_sort.hpp>

int main(int argc [[maybe_unused]], char const *argv [[maybe_unused]][]) {
  std::vector<std::vector<int>> vectors = {
      {
          {1, 2, -2, 5, -8, -19, -8, 0},
          {-9, 0, 12, 6},
          {1},
          {2, 1},
          {1, 2, -2, 0, -8, 19, 8, 0},
          {},
      },
  };

  std::cout << "insertion sort" << std::endl;

  for (auto &v : vectors) {
    xroost::algo::insertion_sort(v);
    std::ranges::copy(v, std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    BOOST_TEST(boost::algorithm::is_increasing(v));
  }

  return boost::report_errors();
}
