#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

#if __cplusplus < 202002UL || !__has_cpp_attribute(no_unique_address)
#error "cannot compile with [[no_unique_address]] atribute unsupported"
#endif

template <typename T, typename Deleter = std::default_delete<T>>
class unique_ptr_cpp20 {
public:
  unique_ptr_cpp20() = default;
  explicit unique_ptr_cpp20(T *p, Deleter d = Deleter()) noexcept(
      std::is_nothrow_move_assignable_v<Deleter> ||
      std::is_nothrow_copy_assignable_v<Deleter>)
      : p_(p), d_(std::move_if_noexcept(d)) {}

  ~unique_ptr_cpp20() noexcept(
      noexcept(std::declval<Deleter>().operator()(static_cast<T *>(nullptr)))) {
    if (p_)
      d_(p_);
  }

  unique_ptr_cpp20(unique_ptr_cpp20 const &) = delete;
  unique_ptr_cpp20 &operator=(unique_ptr_cpp20 const &) = delete;

  unique_ptr_cpp20 &operator=(unique_ptr_cpp20 &&other) noexcept(
      std::is_nothrow_move_assignable_v<Deleter> ||
      std::is_nothrow_copy_assignable_v<Deleter>) {
    if (this != &other) {
      p_ = std::exchange(other.p_, nullptr);
      d_ = std::move_if_noexcept(other.d_);
    }
    return *this;
  }

  unique_ptr_cpp20(unique_ptr_cpp20 &&other) noexcept(
      std::is_nothrow_move_assignable_v<unique_ptr_cpp20>) {
    if (this != &other)
      *this = std::move(other);
  }

  T *operator->() const noexcept { return p_; }
  T &operator*() const noexcept { return *p_; }

private:
  T *p_{nullptr};
  [[no_unique_address]] Deleter d_{};
};

int main(int argc, char const *argv[]) {
  unique_ptr_cpp20<int> p1{new int{42}};
  static_assert(sizeof(p1) == sizeof(std::addressof(*p1)));
  std::cout << sizeof(p1) << std::endl;
  std::cout << sizeof(std::addressof(*p1)) << std::endl;

  unique_ptr_cpp20<int, std::function<void(int *)>> p2{new int{42},
                                                 std::default_delete<int>{}};
  static_assert(sizeof(p2) != sizeof(std::addressof(*p2)));
  std::cout << sizeof(p2) << std::endl;
  std::cout << sizeof(std::addressof(*p2)) << std::endl;

  return 0;
}
