#include <cassert>
#include <cstdint>

#include <concepts>
#include <list>
#include <optional>
#include <ranges>
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
class generic_lfu {
public:
  using key_type = Key;
  using value_type = T;
  using key_hash = KeyHash;
  using key_equal = KeyEqual;

  static std::unique_ptr<generic_lfu> create(uint64_t cache_len);

private:
  using node_type = std::pair<key_type, value_type>;
  using lru_list_value = node_type;
  using lru_list = std::list<lru_list_value>;
  using lfu_list_value = std::pair<uint64_t, lru_list>;
  using lfu_list = std::list<lfu_list_value>;
  using cache_value =
      std::pair<typename lfu_list::iterator, typename lru_list::iterator>;
  struct stored_value {
    mutable cache_value value;
  };
  using cache = std::unordered_map<key_type, stored_value, key_hash, key_equal>;

public:
  explicit generic_lfu(uint64_t len_max) : len_max_(len_max) {
    assert(this->len_max() > 0);

    cache_.reserve(this->len_max());
  }
  ~generic_lfu() = default;

  generic_lfu(generic_lfu const &) = delete;
  generic_lfu &operator=(generic_lfu const &) = delete;

  generic_lfu(generic_lfu &&) = default;
  generic_lfu &operator=(generic_lfu &&) = default;

  uint64_t len_max() const { return len_max_; }

  bool exists(key_type const &key) const { return cache_.contains(key); }

  std::optional<value_type> get(key_type const &key) const {
    // finding in the hash table is O(1) complexity
    if (auto i = cache_.find(key); i != cache_.end()) {
      // touching is O(1) complexity, see below
      i->second.value = touch(i->second.value);
      return i->second.value.second->second;
    }
    // return -1 in case of unfound
    return std::nullopt;
  }

  std::optional<node_type> put(node_type const &node) {
    auto evicted_value{std::optional<std::pair<key_type, value_type>>{}};

    // finding in the hash table is O(1) operation
    if (auto i = cache_.find(node.first); i != cache_.end()) {
      // touching is O(1) complexity, see below
      i->second.value = touch(i->second.value);
      i->second.value.second->second = node.second;
      return evicted_value;
    } else if (cache_full()) {
      // LFU node detaching is O(1) complexity, see below
      auto [key, value] = lfu_detach();
      // erasing from the cache is O(1) complexity
      cache_.erase(key);
      evicted_value.emplace(std::move(key), std::move(value));
    }

    // LFU node attaching is O(1) complexity, see below
    // inserting a new pair {key, value} in the hash table is O(1) complexity
    cache_[node.first].value = lfu_attach({node.first, node.second});

    return evicted_value;
  }

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
  void invalidate(cache::iterator it) {
    lfu_detach(it->second.value);
    cache_.erase(it);
  }

  bool cache_full() const { return !(cache_.size() < len_max()); }

  // LFU node attaching is O(1) complexity
  [[nodiscard]] cache_value lfu_attach(node_type &&v) {
    if (lfu_list_.empty() || lfu_list_.front().first > 1)
      lfu_list_.push_front({1, {}});

    auto &lfu_list_first{lfu_list_.front()};
    assert(1 == lfu_list_first.first);
    auto &lfu_lru_list{lfu_list_first.second};
    lfu_lru_list.push_front(std::move(v));

    return {lfu_list_.begin(), lfu_lru_list.begin()};
  }

  // LFU node detaching is O(1) complexity
  node_type lfu_detach(cache_value cv) {
    auto n{*cv.second};
    cv.first->second.erase(cv.second);
    if (cv.first->second.empty())
      lfu_list_.erase(cv.first);
    return n;
  }

  node_type lfu_detach() {
    return lfu_detach(
        {lfu_list_.begin(), std::prev(lfu_list_.front().second.end())});
  }

  // touching is O(1) complexity
  cache_value touch(cache_value cv) const {
    auto [lfu_lru_old_it, node_it]{cv};
    auto lfu_lru_next_it{std::next(lfu_lru_old_it)};

    if (lfu_list_.end() == lfu_lru_next_it ||
        (lfu_lru_old_it->first + 1 < lfu_lru_next_it->first)) {
      lfu_lru_next_it =
          lfu_list_.insert(lfu_lru_next_it, {lfu_lru_old_it->first + 1, {}});
    }

    auto &lfu_lru_list_old{lfu_lru_old_it->second};
    auto &lfu_lru_list_new{lfu_lru_next_it->second};
    lfu_lru_list_new.splice(lfu_lru_list_new.empty() ? lfu_lru_list_new.end()
                                                     : lfu_lru_list_new.begin(),
                            lfu_lru_list_old, node_it);
    if (lfu_lru_list_old.empty())
      lfu_list_.erase(lfu_lru_old_it);

    return {lfu_lru_next_it, lfu_lru_list_new.begin()};
  }

private:
  uint64_t len_max_;
  mutable lfu_list lfu_list_;
  cache cache_;
};

template <typename Key, typename T, typename KeyHash, typename KeyEqual>
  requires std::equivalence_relation<KeyEqual, Key, Key>
auto generic_lfu<Key, T, KeyHash, KeyEqual>::create(uint64_t cache_len)
    -> std::unique_ptr<generic_lfu> {
  auto cache{std::unique_ptr<generic_lfu>{}};
  if (cache_len) {
    cache =
        std::unique_ptr<generic_lfu>{new generic_lfu<key_type, T>{cache_len}};
  }
  return cache;
}
