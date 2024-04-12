#include <cstdlib>

#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <vector>
#include <typeinfo>
#include <format>

template <typename T, size_t Alignment = alignof(T)> class Allocator {
public:
  static_assert(0 == (Alignment & (Alignment - 1)),
                "Alignment must be a power of 2");
  static_assert(alignof(T) <= Alignment,
                "Alignment must be at least alignof(T)");
  template <class U> struct rebind {
    using other = Allocator<U, Alignment /*std::max(alignof(U), Alignment)*/>;
  };

  using value_type = T;

  Allocator() = default;
  ~Allocator() = default;

  template <typename U, size_t A>
  constexpr explicit Allocator(Allocator<U, A> const &other) noexcept {}

  Allocator(Allocator const &other) = default;
  Allocator(Allocator &&other) = default;

  T *allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      throw std::bad_array_new_length();

    std::cout << std::format("allocate called for {}, required {} bytes {} times", typeid(T).name(), sizeof(T), n) << std::endl;

    if (auto *p = static_cast<T *>(aligned_alloc(Alignment, sizeof(T) * n)))
      return p;

    throw std::bad_alloc();
  }

  void deallocate(T *p, std::size_t n) noexcept { std::free(p); }
};

int main(int argc, char const *argv[]) try {
  Allocator<int, 1 << 28> alloc{};
  std::vector<std::shared_ptr<int>> v;
  for (int i = 0; i < 1 << 4; ++i) {
    v.push_back(std::allocate_shared<int>(alloc, 0));
    std::cout << v.back() << std::endl;
  }
  return 0;
} catch (std::exception const &ex) {
  std::cerr << ex.what() << std::endl;
  return 1;
}
