#include <array>
#include <iostream>
#include <memory>
#include <ranges>
#include <stack>
#include <string>
#include <string_view>
#include <type_traits>

struct info {
  std::string first_name;
  std::string second_name;

  friend std::ostream &operator<<(std::ostream &out, info const &info) {
    out << info.second_name << " " << info.first_name;
    return out;
  }
};

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
  template <typename... Args>
  bool insert_at_once(std::string_view name, Args &&...args) {
    if (name.empty())
      return false;

    auto *node = &entry_[name[0] - 'A'];
    for (auto c : name.substr(1)) {
      if (!(*node))
        *node = std::make_unique<db_node<T>>();
      node = &(*node)->next[c - 'A'];
    }

    if (!(*node))
      *node = std::make_unique<db_node<T>>();

    auto &val = (*node)->val;
    if (val)
      return false;

    val = std::make_unique<T>(std::forward<Args>(args)...);

    return true;
  }

public:
  template <typename U = T,
            std::enable_if_t<std::is_same_v<std::decay_t<U>, T>, int> = 0>
  bool insert(std::string_view name, U &&t) {
    return insert_at_once(name, std::forward<U>(t));
  }

  template <typename... Args>
  bool emplace(std::string_view name, Args &&...args) {
    return insert_at_once(name, std::forward<Args>(args)...);
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

int main(int argc, char const *argv[]) {
  info const users[] = {
      {"Denis", "Pronin"},    {"Kirill", "Pinegin"},
      {"Elena", "Stepanova"}, {"Nikita", "Besperstov"},
      {"Feliks", "Feliks"},   {"Andrey", "Dubasov"},
      {"Andrey", "Abrakhin"}, {"Ilia", "Zubov"},
      {"Ilya", "Morozov"},    {"Andrew", "Andrew"},
      {"Slava", "Palchikov"}, {"Pavel", "Melkozerov"},
      {"Vlad", "Saburov"},    {"Dmitry", "Smirnov"},
      {"Leonid", "Geller"},   {"Yaroslav", "DeLaPenia"},
  };

  db<info> db;

  for (auto const &user : users)
    db.insert(user.second_name, user);

  db.insert("Ivanov", info{"Ivan", "Ivanov"});
  db.insert("Sidorov", info{"Petr", "Sidorov"});

  db.print(std::cout);

  return 0;
}
