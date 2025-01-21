#include <memory>
#include <utility>
#include <iostream>
#include <utility>

#include <boost/container/flat_map.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/list.hpp>

class Animal {
public:
	Animal(std::string const &name, bool d) noexcept : name_(name), b_dang_(d) {}
	~Animal() noexcept = default;

	Animal(Animal const &that)
		: name_(that.name_)
		, b_dang_(that.b_dang_)
	{
		std::cout << "called copy constructor" << std::endl;
	}

	Animal(Animal &&that)
		: name_(std::move(that.name_))
		, b_dang_(that.b_dang_)
	{
		std::cout << "called move constructor" << std::endl;
	}

	Animal& operator=(Animal const &that)
	{
		name_ = that.name_;
		b_dang_ = that.b_dang_;
		std::cout << "called copy assignment operator" << std::endl;
		return *this;
	}

	Animal& operator=(Animal &&that)
	{
		name_ = std::move(that.name_);
		b_dang_ = that.b_dang_;
		std::cout << "called move assignment operator" << std::endl;
		return *this;
	}

	bool is_dangerous() const { return b_dang_; }
	std::string const& refname() const { return name_; }

	const size_t animal_number = NEXT_ANIMAL_NUMBER++;

private:
	std::string name_;
	bool b_dang_;
	static size_t NEXT_ANIMAL_NUMBER;
};

size_t Animal::NEXT_ANIMAL_NUMBER = 0;

int main(int argc, char const *argv[]) {
	// boost::container::flat_map<std::string, Animal> animals;
	// animals.emplace("lion", Animal{"lion", true});
	// animals.emplace("tiger", Animal{"tiger", true});
	// animals.emplace("bird", Animal{"bird", false});
	// animals.emplace("1", Animal{"rabbit", false});
	// animals.emplace("2", Animal{"hypo", true});

	boost::container::vector<Animal> vec;
	vec.emplace_back("lion", true);
	std::cout << std::endl;
	vec.emplace_back("tiger", true);
	std::cout << std::endl;
	vec.emplace_back("bird", false);
	std::cout << std::endl;
	vec.emplace_back("rabbit", false);
	std::cout << std::endl;
	vec.emplace_back("hypo", true);
	std::cout << std::endl;

	boost::container::list<int> lst;
	return 0;
}
