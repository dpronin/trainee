#include <cstddef>
#include <cstdint>

#include <bit>
#include <concepts>
#include <format>
#include <iostream>
#include <typeindex>

#include <xroost/endian/endian.hpp>

namespace {

void foo() {}

template <typename T, typename... Args>
  requires std::integral<T>
void foo(T v, Args... args) {
  std::println("value {} -> {:#x}", std::type_index(typeid(v)).name(), v);
  std::println("\tnative_to_little -> {:#x}",
               xroost::endian::native_to_little(v));
  std::println("\tnative_to_big -> {:#x}", xroost::endian::native_to_big(v));
  if constexpr (std::endian::little == std::endian::native)
    std::println("\tlittle_to_big -> {:#x}", xroost::endian::little_to_big(v));
  else
    std::println("\tbig_to_little -> {:#x}", xroost::endian::big_to_little(v));
  foo(args...);
}

} // namespace

int main(int argc, char const *argv[]) {
  std::cout << std::format("current platform is {} endian",
                           std::endian::big == std::endian::native ? "big"
                                                                   : "little")
            << std::endl;
  foo(UINT8_C(0x11), UINT16_C(0x1122), UINT32_C(0x11223344),
      UINT64_C(0x1122334455667788), INT8_C(0x17), INT16_C(0x1877),
      INT32_C(0x1E8899AA77), INT64_C(0x1E8899AABBCCDD77));
}
