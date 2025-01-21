#include <iostream>
#include <deque>
#include <string>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/range/algorithm/for_each.hpp>

int main(int argc, char const *argv[]) {
    std::deque<std::string> deq = { "photo.jpg",
        "project.txt",
        "hover.cpp",
        "you.heic",
        "my.json",
    };
    boost::range::for_each(deq, [](auto &&str) noexcept {
        auto path = boost::filesystem::path(str);
        std::cout << path.stem() << std::endl;
        std::cout << path.extension() << std::endl;
    });

    return 0;
}
