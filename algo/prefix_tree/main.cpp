#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ostream>
#include <string>
#include <string_view>

#include "prefix_tree.hpp"

namespace {

struct info {
  std::string first_name;
  std::string second_name;

  std::string_view index() const { return second_name; }

  friend std::ostream &operator<<(std::ostream &out, info const &info) {
    return out << info.second_name << " " << info.first_name;
  }
};

} // namespace

int main(int argc, char const *argv[]) {
  db<info> db{
      {"Denis", "Pronin"},    {"Kirill", "Pinegin"},
      {"Elena", "Stepanova"}, {"Nikita", "Besperstov"},
      {"Andrey", "Dubasov"},  {"Andrey", "Abrakhin"},
      {"Ilia", "Zubov"},      {"Ilya", "Morozov"},
      {"Slava", "Palchikov"}, {"Pavel", "Melkozerov"},
      {"Vlad", "Saburov"},    {"Dmitry", "Smirnov"},
      {"Leonid", "Geller"},   {"Yaroslav", "DeLaPenia"},
  };

  db.emplace("Ivan", "Ivanov");
  db.emplace("Petr", "Sidorov");

  db.print(std::cout);

  return 0;
}
