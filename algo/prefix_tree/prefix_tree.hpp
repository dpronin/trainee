#include <array>
#include <concepts>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <ranges>
#include <stack>
#include <type_traits>

template <typename T> struct db_node;

template <typename T> using db_node_t = std::unique_ptr<db_node<T>>;
template <typename T>
using db_nodes_t = std::array<db_node_t<T>, 'z' - 'A' + 1>;

template <typename T> using db_val_t = std::unique_ptr<T>;

template <typename T> struct db_node {
  db_nodes_t<T> next;
  db_val_t<T> val;
};

template <typename T> class db {
private:
  template <typename... Args> bool insert_at_once(Args &&...args) {
    auto val{std::make_unique<T>(std::forward<Args>(args)...)};

    auto const index{val->index()};
    static_assert(
        std::same_as<std::decay_t<decltype(index)>, std::string_view>);

    if (index.empty())
      return false;

    auto *node = &entry_[index[0] - 'A'];
    for (auto c : index | std::views::drop(1)) {
      if (!(*node))
        *node = std::make_unique<db_node<T>>();
      node = &(*node)->next[c - 'A'];
    }

    if (!(*node))
      *node = std::make_unique<db_node<T>>();

    if (!(*node)->val) {
      (*node)->val = std::move(val);
      return true;
    }

    return false;
  }

public:
  template <std::input_iterator InputIt> db(InputIt first, InputIt last) {
    for (; first != last; ++first)
      insert_at_once(*first);
  }

  template <std::ranges::input_range Range>
  db(Range &&rng) : db(std::ranges::begin(rng), std::ranges::end(rng)) {}

  db(std::initializer_list<T> init) : db(init.begin(), init.end()) {}

  bool insert(std::same_as<T> auto &&t) {
    return insert_at_once(std::forward<decltype(t)>(t));
  }

  template <typename... Args> bool emplace(Args &&...args) {
    return insert_at_once(std::forward<Args>(args)...);
  }

  void print(std::ostream &out) const {
    std::stack<db_node<T> const *> nodes;

    for (auto const &node : entry_ | std::views::reverse) {
      if (node)
        nodes.push(node.get());
    }

    while (!nodes.empty()) {
      auto const &node = nodes.top();

      nodes.pop();

      if (auto const &val = node->val)
        out << *val << std::endl;

      for (auto const &nnode : node->next | std::views::reverse) {
        if (nnode)
          nodes.push(nnode.get());
      }
    }
  }

private:
  db_nodes_t<T> entry_;
};
