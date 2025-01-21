#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <boost/optional.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

struct animal
{
    std::string name;
    int legs;

    animal(std::string const &n, int l) : name(n), legs(l) {}
};

class animals_container
{
public:
    void add(animal &&a)
    {
        auto res = map_.insert({a.name, a.legs});
        if (!res.second) throw std::runtime_error("couldn't insert an item");
    }

    boost::optional<animal> find_by_name(std::string const &name) const noexcept
    {
        boost::optional<animal> a;
        if (auto res = map_.left.find(name); res != map_.left.end()) a = animal{res->first, res->second};
        return a;
    }

    std::vector<animal> find_by_legs(int from, int to) const noexcept
    {
        auto range = map_.right.range([=](int item){ return from <= item; }, [=](int item){ return item <= to; });
        std::vector<animal> v;
        std::for_each(range.first, range.second, [&v](auto &&item){ v.emplace_back(item.second, item.first); });
        return v;
    }
private:
    boost::bimap<boost::bimaps::set_of<std::string>, boost::bimaps::multiset_of<int>> map_;
};

int main()
{
    animals_container animals;
    animals.add({ "cat", 4 });
    animals.add({ "ant", 6 });
    animals.add({ "spider", 8 });
    animals.add({ "shark", 0 });

    auto shark = animals.find_by_name("shark");
    if (shark)
        std::cout << "shark has " << shark->legs << " legs\n";

    auto animals_with_4_to_6_legs = animals.find_by_legs(4, 7);
    for (auto animal : animals_with_4_to_6_legs)
        std::cout << animal.name << " has " << animal.legs << " legs\n";

}
