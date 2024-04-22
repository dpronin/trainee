#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <limits>
#include <memory>
#include <random>
#include <ranges>
#include <type_traits>
#include <vector>

#include "flat_lru.hpp"

using namespace testing;

namespace {

using cache = flat_lru<uint64_t, std::byte>;

template <typename T>
concept is_trivially_copyable = std::is_trivially_copyable_v<T>;

constexpr inline auto make_random_bytes_generator = [] {
  return [rd = std::random_device{}] mutable {
    return static_cast<std::byte>(rd());
  };
};

inline std::unique_ptr<std::byte[]> make_unique_for_overwrite_bytes(size_t sz) {
  return std::make_unique_for_overwrite<std::byte[]>(sz);
}

inline std::unique_ptr<std::byte[]> make_unique_randomized_bytes(size_t sz) {
  auto buf{std::make_unique_for_overwrite<std::byte[]>(sz)};
  auto gen = make_random_bytes_generator();
  std::generate_n(buf.get(), sz, std::ref(gen));
  return buf;
}

template <is_trivially_copyable T>
inline auto duplicate_unique(std::unique_ptr<T[]> const &p, size_t sz) {
  auto p_dup{std::make_unique_for_overwrite<T[]>(sz)};
  std::copy_n(p.get(), sz, p_dup.get());
  return p_dup;
}

TEST(LRUCache, CreateValid) {
  constexpr auto kCacheLenMax{1uz};
  constexpr auto kCacheItemSz{1uz};

  auto cache_not_null{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache_not_null);

  EXPECT_EQ(cache_not_null->len_max(), kCacheLenMax);
  EXPECT_EQ(cache_not_null->item_sz(), kCacheItemSz);
}

TEST(LRUCache, CreateInvalid) {
  constexpr auto kCacheLenMax{1uz};
  constexpr auto kCacheItemSz{1uz};

  auto cache_nulls{
      std::array{
          cache::create(0uz, kCacheItemSz),
          cache::create(kCacheLenMax, 0uz),
      },
  };

  for (auto const &cache_null : cache_nulls)
    EXPECT_FALSE(cache_null);
}

TEST(LRUCache, InsertAndFindNonExistent) {
  constexpr auto kCacheLenMax{32uz};
  constexpr auto kCacheItemSz{1u};

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  for (auto key : std::views::iota(0uz, kCacheLenMax))
    cache->update({key, make_unique_for_overwrite_bytes(kCacheItemSz)});

  auto rd{std::random_device{}};
  auto unexistent_keys_random_gen{
      std::uniform_int_distribution<uint64_t>{
          kCacheLenMax,
          std::numeric_limits<uint64_t>::max(),
      },
  };

  for (auto i [[maybe_unused]] : std::views::iota(0, 1024)) {
    auto const unexistent_key{unexistent_keys_random_gen(rd)};
    EXPECT_FALSE(cache->exists(unexistent_key));
    auto const buf{cache->find(unexistent_key)};
    EXPECT_TRUE(buf.empty());
  }
}

TEST(LRUCache, InsertAndFind) {
  constexpr auto kCacheLenMax{32uz};
  constexpr auto kCacheItemSz{16uz};

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  auto bufs_pairs{
      std::vector<std::pair<uint64_t, std::unique_ptr<std::byte[]>>>{
          cache->len_max(),
      },
  };
  std::ranges::generate(
      bufs_pairs, [i = 0uz] mutable -> decltype(bufs_pairs)::value_type {
        return {i++, make_unique_randomized_bytes(kCacheItemSz)};
      });

  decltype(bufs_pairs) bufs_pairs_dup{cache->len_max()};
  std::ranges::transform(
      bufs_pairs, bufs_pairs_dup.begin(),
      [](auto const &buf_pair) -> decltype(bufs_pairs)::value_type {
        return {
            buf_pair.first,
            duplicate_unique(buf_pair.second, kCacheItemSz),
        };
      });

  auto rd{std::random_device{}};
  std::ranges::shuffle(bufs_pairs, rd);

  for (auto &buf_pair : bufs_pairs) {
    auto const evicted_value{cache->update(std::move(buf_pair))};
    EXPECT_FALSE(evicted_value.has_value());
  }

  for (auto const &[key, buf_dup] : bufs_pairs_dup) {
    auto const buf{cache->find(key)};
    EXPECT_THAT(buf, ElementsAreArray(buf_dup.get(), kCacheItemSz));
  }
}

TEST(LRUCache, FindMutableAndCheckMutation) {
  constexpr auto kCacheLenMax{32uz};
  constexpr auto kCacheItemSz{16uz};

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  auto bufs_pairs{
      std::vector<std::pair<uint64_t, std::unique_ptr<std::byte[]>>>{
          cache->len_max(),
      },
  };
  std::ranges::generate(
      bufs_pairs, [i = 0uz] mutable -> decltype(bufs_pairs)::value_type {
        return {i++, std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz)};
      });

  decltype(bufs_pairs) bufs_pairs_dup{cache->len_max()};
  std::ranges::transform(
      bufs_pairs, bufs_pairs_dup.begin(),
      [](auto const &buf_pair) -> decltype(bufs_pairs)::value_type {
        return {buf_pair.first, std::unique_ptr<std::byte[]>{}};
      });

  auto rd{std::random_device{}};
  std::ranges::shuffle(bufs_pairs, rd);

  for (auto &buf_pair : bufs_pairs) {
    auto const evicted_value{cache->update(std::move(buf_pair))};
    EXPECT_FALSE(evicted_value.has_value());
  }

  for (auto &[key, buf_dup] : bufs_pairs_dup) {
    auto const buf{cache->find_mutable(key)};
    ASSERT_FALSE(buf.empty());
    ASSERT_EQ(buf.size(), kCacheItemSz);
    std::copy_n(make_unique_randomized_bytes(kCacheItemSz).get(), kCacheItemSz,
                buf.begin());
    buf_dup = std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz);
    std::ranges::copy(buf, buf_dup.get());
  }

  for (auto const &[key, buf_dup] : bufs_pairs_dup) {
    auto const buf{cache->find(key)};
    EXPECT_THAT(buf, ElementsAreArray(buf_dup.get(), kCacheItemSz));
  }
}

TEST(LRUCache, Invalidate) {
  constexpr auto kCacheLenMax{32uz};
  constexpr auto kCacheItemSz{1uz};

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  auto bufs_pairs{
      std::vector<std::pair<uint64_t, std::unique_ptr<std::byte[]>>>{
          cache->len_max(),
      },
  };
  std::ranges::generate(
      bufs_pairs, [i = 0uz] mutable -> decltype(bufs_pairs)::value_type {
        return {i++, std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz)};
      });

  auto rd{std::random_device{}};
  std::ranges::shuffle(bufs_pairs, rd);

  for (auto &[key, buf] : bufs_pairs)
    cache->update({key, std::move(buf)});

  for (auto keys{std::vector<uint64_t>{}}; !bufs_pairs.empty();) {
    for (auto key : bufs_pairs | std::views::keys)
      EXPECT_TRUE(cache->exists(key));

    auto const key{bufs_pairs.back().first};
    bufs_pairs.pop_back();

    cache->invalidate(key);

    keys.push_back(key);
    for (auto key_prev : keys | std::views::reverse) {
      EXPECT_FALSE(cache->exists(key_prev));
      auto const buf{cache->find(key_prev)};
      EXPECT_TRUE(buf.empty());
    }
  }
}

TEST(LRUCache, InvalidateRange) {
  constexpr auto kCacheLenMax{32uz};
  constexpr auto kCacheItemSz{1uz};

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  auto bufs_pairs{
      std::vector<std::pair<uint64_t, std::unique_ptr<std::byte[]>>>{
          cache->len_max(),
      },
  };
  std::ranges::generate(
      bufs_pairs, [i = 0uz] mutable -> decltype(bufs_pairs)::value_type {
        return {i++, std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz)};
      });

  auto rd{std::random_device{}};
  std::ranges::shuffle(bufs_pairs, rd);

  for (auto &[key, buf] : bufs_pairs)
    cache->update({key, std::move(buf)});

  for (auto key : bufs_pairs | std::views::keys)
    EXPECT_TRUE(cache->exists(key));

  cache->invalidate_range({0uz, bufs_pairs.size()});

  for (auto key : bufs_pairs | std::views::keys) {
    EXPECT_FALSE(cache->exists(key));
    auto const buf{cache->find(key)};
    EXPECT_TRUE(buf.empty());
  }
}

TEST(LRUCache, EvictValidEntriesInOrder) {
  constexpr auto kCacheLenMax{32uz};
  constexpr auto kCacheItemSz{1uz};

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{
        cache->update(
            {key, std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz)}),
    };
    ASSERT_FALSE(evicted_value.has_value());
  }

  for (auto key : std::views::iota(kCacheLenMax, 2 * kCacheLenMax)) {
    auto const evicted_value{
        cache->update(
            {key, std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz)}),
    };
    ASSERT_TRUE(evicted_value.has_value());
    EXPECT_EQ(evicted_value->first, key - kCacheLenMax);
  }
}

TEST(LRUCache, EvictInsertInvalidatedEntry) {
  constexpr auto kCacheLenMax{8uz};
  constexpr auto kCacheItemSz{1uz};
  constexpr auto kKeyToVerify{6uz};
  static_assert(kKeyToVerify < kCacheLenMax);

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{
        cache->update({
            key,
            std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz),
        }),
    };
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToVerify);
  ASSERT_FALSE(cache->exists(kKeyToVerify));

  auto const evicted_value{
      cache->update({kKeyToVerify, std::make_unique_for_overwrite<std::byte[]>(
                                       kCacheItemSz)}),
  };
  EXPECT_FALSE(evicted_value.has_value());
}

TEST(LRUCache, EvictInvalidatedEntryInsertOffByOneEntry) {
  constexpr auto kCacheLenMax{8uz};
  constexpr auto kCacheItemSz{1uz};
  constexpr auto kKeyToInvalidate{6uz};
  static_assert(kKeyToInvalidate < kCacheLenMax);
  constexpr auto kKeyStride{2uz};
  static_assert(kKeyStride > 1uz,
                "key stride must not be 1, there must be a gap in between");

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{
        cache->update({
            kKeyStride * key,
            std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz),
        }),
    };
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToInvalidate);
  ASSERT_FALSE(cache->exists(kKeyToInvalidate));

  auto const evicted_value{
      cache->update({
          kKeyToInvalidate - 1,
          std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz),
      }),
  };
  EXPECT_FALSE(evicted_value.has_value());
}

TEST(LRUCache, InvalidateLessInsertEntryEvictInvalidated) {
  constexpr auto kCacheLenMax{16uz};
  constexpr auto kCacheItemSz{1uz};
  constexpr auto kKeyToInsert{7uz};
  static_assert((kKeyToInsert < kCacheLenMax) && (kKeyToInsert % 2));
  constexpr auto kKeyToInvalidate{4uz};
  static_assert((kKeyToInvalidate < kKeyToInsert) &&
                0 == (kKeyToInvalidate % 2));
  constexpr auto kKeyStride{2uz};
  static_assert(0 == (kKeyStride % 2), "key stride must be even");

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{
        cache->update({
            kKeyStride * key,
            std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz),
        }),
    };
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToInvalidate);
  ASSERT_FALSE(cache->exists(kKeyToInvalidate));

  auto const evicted_value{
      cache->update({
          kKeyToInsert,
          std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz),
      }),
  };
  EXPECT_FALSE(evicted_value.has_value());
}

TEST(LRUCache, InvalidateGreaterInsertEntryEvictInvalidated) {
  constexpr auto kCacheLenMax{16uz};
  constexpr auto kCacheItemSz{1uz};
  constexpr auto kKeyToInsert{7uz};
  static_assert((kKeyToInsert < kCacheLenMax) && (kKeyToInsert % 2));
  constexpr auto kKeyToInvalidate{12uz};
  static_assert((kKeyToInvalidate > kKeyToInsert) &&
                0 == (kKeyToInvalidate % 2));
  constexpr auto kKeyStride{2uz};
  static_assert(0 == (kKeyStride % 2), "key stride must be even");

  auto cache{cache::create(kCacheLenMax, kCacheItemSz)};
  ASSERT_TRUE(cache);

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{
        cache->update({
            kKeyStride * key,
            std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz),
        }),
    };
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToInvalidate);
  ASSERT_FALSE(cache->exists(kKeyToInvalidate));

  auto const evicted_value{
      cache->update({
          kKeyToInsert,
          std::make_unique_for_overwrite<std::byte[]>(kCacheItemSz),
      }),
  };
  EXPECT_FALSE(evicted_value.has_value());
}

} // namespace
