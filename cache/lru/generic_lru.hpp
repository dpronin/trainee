#pragma once

#include <cassert>
#include <cstdint>

#include <concepts>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <unordered_map>
#include <utility>

/* clang-format off */
template <
  typename Key,
  typename T,
  typename KeyHash = std::hash<Key>,
  typename KeyEqual = std::ranges::equal_to
>
/* clang-format on */
  requires std::equivalence_relation<KeyEqual, Key, Key>
class generic_lru {
public:
  using key_type = Key;
  using data_type = std::unique_ptr<T[], std::function<void(T[])>>;
  using key_hash = KeyHash;
  using key_equal = KeyEqual;

  static std::unique_ptr<generic_lru> create(uint64_t cache_len,
                                             uint64_t cache_item_sz);

private:
  using value_type = std::pair<key_type, data_type>;
  using recent_list = std::list<value_type>;
  using cache = std::unordered_map<Key, typename recent_list::iterator, KeyHash,
                                   KeyEqual>;

public:
  explicit generic_lru(uint64_t len_max, uint64_t item_sz)
      : cache_len_max_(len_max), cache_item_sz_(item_sz) {
    assert(this->len_max() > 0);
    assert(this->item_sz() > 0);

    cache_.reserve(this->len_max());
  }
  ~generic_lru() = default;

  generic_lru(generic_lru const &) = delete;
  generic_lru &operator=(generic_lru const &) = delete;

  generic_lru(generic_lru &&) = default;
  generic_lru &operator=(generic_lru &&) = default;

  uint64_t item_sz() const { return cache_item_sz_; }
  uint64_t len_max() const { return cache_len_max_; }

  std::span<T const> find(key_type const &key) const {
    if (auto it{cache_.find(key)}; cache_.end() != it) {
      touch(it->second);
      return data_view(*it->second);
    }
    return {};
  }

  std::span<T> find_mutable(key_type const &key) {
    return const_span_cast(find(key));
  }

  std::optional<std::pair<key_type, data_type>>
  update(std::pair<key_type, data_type> value);

  bool exists(key_type const &key) const { return cache_.contains(key); }

  void invalidate(key_type const &key) {
    if (auto it{cache_.find(key)}; cache_.end() != it)
      invalidate(it);
  }

  template <std::integral U = Key>
  void invalidate_range(std::pair<U, U> const &range) {
    assert(range.first < range.second);
    for (auto key : std::views::iota(range.first, range.second))
      invalidate(key);
  }

private:
  template <typename U>
  static std::span<U> const_span_cast(std::span<U const> from) {
    return std::span{const_cast<U *>(from.data()), from.size()};
  }

  auto data_view(value_type const &value) const {
    return std::span{value.second.get(), item_sz()};
  }

  void invalidate(cache::iterator it) {
    recent_list_.erase(it->second);
    cache_.erase(it);
  }

  bool is_valid(value_type const &value) const {
    return value.second.refs != len_max();
  }

  void touch(recent_list::iterator i) const {
    recent_list_.splice(recent_list_.begin(), recent_list_, i);
  }

  uint64_t cache_len_max_;
  uint64_t cache_item_sz_;

  mutable recent_list recent_list_;
  cache cache_;
};

template <typename Key, typename T, typename KeyHash, typename KeyEqual>
  requires std::equivalence_relation<KeyEqual, Key, Key>
auto generic_lru<Key, T, KeyHash, KeyEqual>::create(uint64_t cache_len,
                                                    uint64_t cache_item_sz)
    -> std::unique_ptr<generic_lru> {
  auto cache{std::unique_ptr<generic_lru>{}};
  if (cache_len && cache_item_sz) {
    cache = std::unique_ptr<generic_lru>{
        new generic_lru<key_type, T>{
            cache_len,
            cache_item_sz,
        },
    };
  }
  return cache;
}

template <typename Key, typename T, typename KeyHash, typename KeyEqual>
  requires std::equivalence_relation<KeyEqual, Key, Key>
auto generic_lru<Key, T, KeyHash, KeyEqual>::update(
    std::pair<key_type, data_type> value)
    -> std::optional<std::pair<key_type, data_type>> {
  auto evicted_value{std::optional<std::pair<key_type, data_type>>{}};

  if (auto it{cache_.find(value.first)}; it != cache_.end()) {
    evicted_value.emplace(
        it->second->first,
        std::exchange(it->second->second, std::move_if_noexcept(value.second)));
    touch(it->second);
    return evicted_value;
  } else if (!(cache_.size() < len_max())) {
    cache_.erase(recent_list_.back().first);
    evicted_value.emplace(std::move_if_noexcept(recent_list_.back()));
    recent_list_.pop_back();
  }

  recent_list_.emplace_front(std::move_if_noexcept(value));
  cache_[recent_list_.begin()->first] = recent_list_.begin();

  return evicted_value;
}
