#include <array>
#include <functional>

#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/unordered_set_hook.hpp>

struct MyType : boost::intrusive::unordered_set_base_hook<> {
  int key;
};

inline bool operator==(MyType const &l, MyType const &r) {
  return l.key == r.key;
}

inline size_t hash_value(MyType const &v) { return std::hash<size_t>{}(v.key); }

int main(int argc [[maybe_unused]], char const *argv [[maybe_unused]][]) {
  using unordered_set = boost::intrusive::unordered_set<MyType>;

  std::array<unordered_set::bucket_type, 100> buckets;
  unordered_set::bucket_traits traits{buckets.data(), buckets.size()};
  unordered_set mySet{traits};

  MyType obj1;
  obj1.key = 1;

  mySet.insert(obj1);
  mySet.erase(mySet.iterator_to(obj1));

  return 0;
}
