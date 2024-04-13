#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <format>

namespace {

template <std::integral T> constexpr T byteswap(T value) noexcept {
  static_assert(std::has_unique_object_representations_v<T>,
                "T may not have padding bits");
  auto array{std::bit_cast<std::array<std::byte, sizeof(T)>>(value)};
  std::ranges::reverse(array);
  return std::bit_cast<T>(array);
}

template <std::integral T> constexpr T big_to_native(T v) noexcept {
  if constexpr (std::endian::little == std::endian::native)
    return byteswap(v);
  else
    return v;
}

template <std::integral T> constexpr T native_to_big(T v) noexcept {
  return big_to_native(v);
}

template <std::integral T> constexpr T little_to_native(T v) noexcept {
  if constexpr (std::endian::big == std::endian::native)
    return byteswap(v);
  else
    return v;
}

template <std::integral T> constexpr T native_to_little(T v) noexcept {
  return little_to_native(v);
}

template <std::integral T> constexpr T little_to_big(T v) noexcept {
  return byteswap(v);
}

template <std::integral T> constexpr T big_to_little(T v) noexcept {
  return byteswap(v);
}

void foo() {}

template <typename T, typename... Args>
  requires std::integral<T>
void foo(T v, Args... args) {
  std::cout << std::hex;
  std::cout << std::format("value {} -> {:#x}", std::type_index(typeid(v)).name(), v) << std::endl;
  std::cout <<  std::format("\tnative_to_little -> {:#x}", native_to_little(v)) << std::endl;
  std::cout <<  std::format("\tnative_to_big -> {:#x}", native_to_big(v)) << std::endl;
  if constexpr (std::endian::little == std::endian::native)
    std::cout <<  std::format("\tlittle_to_big -> {:#x}", little_to_big(v)) << std::endl;
  else
    std::cout <<  std::format("\tbit_to_little -> {:#x}", bit_to_little(v)) << std::endl;
  std::cout << std::dec;
  foo(args...);
}

} // namespace

int main(int argc, char const *argv[]) {
  std::cout << std::format("current platform is {} endian", std::endian::big == std::endian::native ? "big" : "little") << std::endl;
  foo(UINT8_C(0x11),
      UINT16_C(0x1122),
      UINT32_C(0x11223344),
      UINT64_C(0x1122334455667788),
      INT8_C(0x17),
      INT16_C(0x1877),
      INT32_C(0x1E8899AA77),
      INT64_C(0x1E8899AABBCCDD77)
    );
}
