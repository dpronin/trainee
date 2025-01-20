/**
 * Since C++23 std::aligned_storage and relatives/derivatives have been marked
 * as deprecated
 *
 * In this demo I show up how you can elaborate your own aligned_storage based
 * on new well-known C++ features
 *
 * Author: Denis Pronin <dannftk@yandex.ru>
 */

#include <cstddef>

#include <iostream>

#include <print>

#include <xroost/utility/aligned_storage.hpp>

using int_aligned_8 = xroost::aligned_storage_t<int, 8>;

int main(int argc, char const *argv[]) {
  int_aligned_8 i;
  new (&i) int();

  int_aligned_8 j;
  new (&j) int();

  int *iu = &reinterpret_cast<int &>(i);
  *iu = 12;

  int *ju = &reinterpret_cast<int &>(j);
  *ju = 13;

  std::println(std::cout, "{} {}", *iu, *ju);

  return 0;
}
