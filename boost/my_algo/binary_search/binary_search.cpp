#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include <boost/container/static_vector.hpp>

#include <xroost/algo/binary_search.hpp>

int main(int argc [[maybe_unused]], char const *argv [[maybe_unused]][]) {
  boost::container::static_vector<std::vector<int>, 10> vectors = {
      {
          {1, 2, 2, 4, 10, 12, 13, 50},
          {-9, 0, 1, 6},
          {1},
          {1, 1, 2},
          {-1, -2, -2, 0, 8, 19, 88, 100},
          {},
      },
  };

  for (auto const &v : vectors) {
    std::ranges::copy(v, std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    if (auto it{xroost::algo::binary_search(v.begin(), v.end(), 0)};
        v.end() == it) {
      std::cout << "no item 0";
    } else {
      std::cout << "index of item 0 is " << std::distance(v.begin(), it);
    }
    std::cout << std::endl;
  }

  return 0;
}
