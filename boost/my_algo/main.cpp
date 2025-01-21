#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

// #include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/algorithm/count_if.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>

static int minDeletionSize(std::vector<std::string> const &A) {
    return boost::range::count_if(boost::counting_range(0, static_cast<int>(A[0].size())), [&A](auto i){
        auto const column = A | boost::adaptors::transformed([i](auto &s) { return std::string(1, s[i]); });
        return !boost::range::is_sorted(column);
    });
}

int main(int argc, char const *argv[]) {
    std::cout << minDeletionSize({"cba","daf","ghi"}) << std::endl;
    return 0;
}
