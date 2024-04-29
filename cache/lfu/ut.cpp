#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <limits>
#include <memory>
#include <random>
#include <ranges>
#include <vector>

#include "lfu.hpp"

using namespace testing;

namespace {

template <typename T> class LFUCache : public testing::Test {};

using LFUCacheImpls = Types<generic_lfu<uint64_t, uint64_t>>;
TYPED_TEST_SUITE(LFUCache, LFUCacheImpls);

TYPED_TEST(LFUCache, CreateValid) {
  constexpr auto kCacheLenMax{1uz};

  auto cache_not_null{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache_not_null);

  EXPECT_EQ(cache_not_null->len_max(), kCacheLenMax);
}

TYPED_TEST(LFUCache, CreateInvalid) {
  constexpr auto kCacheLenMax{1uz};

  auto cache_nulls{
      std::array{
          TypeParam::create(0uz),
      },
  };

  for (auto const &cache_null : cache_nulls)
    EXPECT_FALSE(cache_null);
}

TYPED_TEST(LFUCache, InsertAndFindNonExistent) {
  constexpr auto kCacheLenMax{32uz};

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  for (auto key : std::views::iota(0uz, kCacheLenMax))
    cache->put({key, kCacheLenMax - key - 1});

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
    auto const value_opt{cache->get(unexistent_key)};
    EXPECT_FALSE(value_opt.has_value());
  }
}

TYPED_TEST(LFUCache, InsertAndFind) {
  constexpr auto kCacheLenMax{32uz};

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto kvs_pairs{
      std::vector<std::pair<uint64_t, uint64_t>>{
          cache->len_max(),
      },
  };
  std::ranges::generate(kvs_pairs, [i = 0uz] mutable {
    auto v{std::pair{i, kCacheLenMax - i - 1}};
    ++i;
    return v;
  });

  auto kvs_pairs_dup{kvs_pairs};

  auto rd{std::random_device{}};
  std::ranges::shuffle(kvs_pairs, rd);

  for (auto const &kv : kvs_pairs) {
    auto const evicted_value{cache->put(kv)};
    ASSERT_FALSE(evicted_value.has_value());
  }

  for (auto [key, value] : kvs_pairs_dup) {
    auto const value_found{cache->get(key)};
    ASSERT_TRUE(value_found.has_value());
    EXPECT_EQ(*value_found, value);
  }
}

TYPED_TEST(LFUCache, Invalidate) {
  constexpr auto kCacheLenMax{32uz};

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto kvs_pairs{
      std::vector<std::pair<uint64_t, uint64_t>>{
          cache->len_max(),
      },
  };
  std::ranges::generate(kvs_pairs, [i = 0uz] mutable {
    auto v{std::pair{i, kCacheLenMax - i - 1}};
    ++i;
    return v;
  });

  auto rd{std::random_device{}};
  std::ranges::shuffle(kvs_pairs, rd);

  for (auto const &kv : kvs_pairs) {
    auto const evicted_value{cache->put(kv)};
    ASSERT_FALSE(evicted_value.has_value());
  }

  for (auto keys{std::vector<uint64_t>{}}; !kvs_pairs.empty();) {
    for (auto key : kvs_pairs | std::views::keys)
      EXPECT_TRUE(cache->exists(key));

    auto const key{kvs_pairs.back().first};
    kvs_pairs.pop_back();

    cache->invalidate(key);

    keys.push_back(key);
    for (auto key_prev : keys | std::views::reverse) {
      EXPECT_FALSE(cache->exists(key_prev));
      auto const value_opt{cache->get(key_prev)};
      EXPECT_FALSE(value_opt.has_value());
    }
  }
}

TYPED_TEST(LFUCache, InvalidateRange) {
  constexpr auto kCacheLenMax{32uz};

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto kvs_pairs{
      std::vector<std::pair<uint64_t, uint64_t>>{
          cache->len_max(),
      },
  };
  std::ranges::generate(kvs_pairs, [i = 0uz] mutable {
    auto v{std::pair{i, kCacheLenMax - i - 1}};
    ++i;
    return v;
  });

  auto rd{std::random_device{}};
  std::ranges::shuffle(kvs_pairs, rd);

  for (auto const &kv : kvs_pairs) {
    auto const evicted_value{cache->put(kv)};
    ASSERT_FALSE(evicted_value.has_value());
  }

  for (auto key : kvs_pairs | std::views::keys)
    ASSERT_TRUE(cache->exists(key));

  cache->invalidate_range({0uz, kvs_pairs.size()});

  for (auto key : kvs_pairs | std::views::keys) {
    EXPECT_FALSE(cache->exists(key));
    auto const value_opt{cache->get(key)};
    EXPECT_FALSE(value_opt.has_value());
  }
}

TYPED_TEST(LFUCache, EvictValidEntriesInOrder) {
  constexpr auto kCacheLenMax{32uz};

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto rd{std::random_device{}};

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{cache->put({key, rd()})};
    ASSERT_FALSE(evicted_value.has_value());
  }

  for (auto key : std::views::iota(kCacheLenMax, 2 * kCacheLenMax)) {
    auto const evicted_value{cache->put({key, rd()})};
    ASSERT_TRUE(evicted_value.has_value());
    EXPECT_EQ(evicted_value->first, key - kCacheLenMax);
  }
}

TYPED_TEST(LFUCache, EvictInsertInvalidatedEntry) {
  constexpr auto kCacheLenMax{8uz};
  constexpr auto kKeyToVerify{6uz};
  static_assert(kKeyToVerify < kCacheLenMax);

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto rd{std::random_device{}};

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{cache->put({key, rd()})};
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToVerify);
  ASSERT_FALSE(cache->exists(kKeyToVerify));

  auto const evicted_value{cache->put({kKeyToVerify, rd()})};
  EXPECT_FALSE(evicted_value.has_value());
}

TYPED_TEST(LFUCache, EvictInvalidatedEntryInsertOffByOneEntry) {
  constexpr auto kCacheLenMax{8uz};
  constexpr auto kKeyToInvalidate{6uz};
  static_assert(kKeyToInvalidate < kCacheLenMax);
  constexpr auto kKeyStride{2uz};
  static_assert(kKeyStride > 1uz,
                "key stride must not be 1, there must be a gap in between");

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto rd{std::random_device{}};

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{cache->put({kKeyStride * key, rd()})};
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToInvalidate);
  ASSERT_FALSE(cache->exists(kKeyToInvalidate));

  auto const evicted_value{cache->put({kKeyToInvalidate - 1, rd()})};
  EXPECT_FALSE(evicted_value.has_value());
}

TYPED_TEST(LFUCache, InvalidateLessInsertEntryEvictInvalidated) {
  constexpr auto kCacheLenMax{16uz};
  constexpr auto kKeyToInsert{7uz};
  static_assert((kKeyToInsert < kCacheLenMax) && (kKeyToInsert % 2));
  constexpr auto kKeyToInvalidate{4uz};
  static_assert((kKeyToInvalidate < kKeyToInsert) &&
                0 == (kKeyToInvalidate % 2));
  constexpr auto kKeyStride{2uz};
  static_assert(0 == (kKeyStride % 2), "key stride must be even");

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto rd{std::random_device{}};

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{cache->put({kKeyStride * key, rd()})};
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToInvalidate);
  ASSERT_FALSE(cache->exists(kKeyToInvalidate));

  auto const evicted_value{cache->put({kKeyToInsert, rd()})};
  EXPECT_FALSE(evicted_value.has_value());
}

TYPED_TEST(LFUCache, InvalidateGreaterInsertEntryEvictInvalidated) {
  constexpr auto kCacheLenMax{16uz};
  constexpr auto kKeyToInsert{7uz};
  static_assert((kKeyToInsert < kCacheLenMax) && (kKeyToInsert % 2));
  constexpr auto kKeyToInvalidate{12uz};
  static_assert((kKeyToInvalidate > kKeyToInsert) &&
                0 == (kKeyToInvalidate % 2));
  constexpr auto kKeyStride{2uz};
  static_assert(0 == (kKeyStride % 2), "key stride must be even");

  auto cache{TypeParam::create(kCacheLenMax)};
  ASSERT_TRUE(cache);

  auto rd{std::random_device{}};

  for (auto key : std::views::iota(0uz, kCacheLenMax)) {
    auto const evicted_value{cache->put({kKeyStride * key, rd()})};
    ASSERT_FALSE(evicted_value.has_value());
  }

  cache->invalidate(kKeyToInvalidate);
  ASSERT_FALSE(cache->exists(kKeyToInvalidate));

  auto const evicted_value{cache->put({kKeyToInsert, rd()})};
  EXPECT_FALSE(evicted_value.has_value());
}

} // namespace
