#include <cstdint>
#include <iostream>
#include <type_traits>

#include <boost/integer.hpp>

template<size_t S>
void exec()
{
    exec<S - 1>();
    std::cout << S << " -> " << std::is_same<typename boost::uint_t<S>::fast, uint32_t>::value << std::endl;
}

template<>
void exec<0>()
{
}

int main(int argc, char const *argv[]) {
    std::cout << std::boolalpha;
    exec<64>();
    return 0;
}
