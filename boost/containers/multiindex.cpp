#include <string>
#include <memory>
#include <utility>
#include <algorithm>
#include <iostream>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>

class Animal {
public:
	Animal(std::string const &name, bool d) noexcept : name_(name), b_dang_(d) {}
	~Animal() noexcept = default;

	bool is_dangerous() const { return b_dang_; }
	std::string const& refname() const { return name_; }

	const size_t animal_number = NEXT_ANIMAL_NUMBER++;

private:
	std::string name_;
	bool b_dang_;
	static size_t NEXT_ANIMAL_NUMBER;
};

size_t Animal::NEXT_ANIMAL_NUMBER = 0;

struct DangCounter {
	void operator()(std::shared_ptr<Animal> const& spAnimal) noexcept {
		if (spAnimal->is_dangerous()) ++dang_counter_;
	}

	size_t operator()() { return dang_counter_; }

private:
	size_t dang_counter_ = 0;
};

// struct tag1{};
// struct tag2{};
// struct tag3{};

int main(int argc, char const *argv[]) {
	using namespace boost::multi_index;
	using animals_container_t = multi_index_container<std::shared_ptr<Animal>,
		indexed_by<hashed_unique<
						// tag<tag1>,
						const_mem_fun<Animal, std::string const&, &Animal::refname>>,
					ordered_unique<
						// tag<tag2>,
						member<Animal, const size_t, &Animal::animal_number>>,
					hashed_non_unique<
						// tag<tag3>,
						const_mem_fun<Animal, bool, &Animal::is_dangerous>>
		>>;
	animals_container_t animals;
	animals.insert(std::make_shared<Animal>("lion", true));
	animals.insert(std::make_shared<Animal>("brid", false));
	animals.insert(std::make_shared<Animal>("tiger", true));
	animals.insert(std::make_shared<Animal>("cat", false));
	animals.insert(std::make_shared<Animal>("dog", true));
	animals.insert(std::make_shared<Animal>("wolf", true));
	animals.insert(std::make_shared<Animal>("rabbit", false));
	animals.insert(std::make_shared<Animal>("shark", false));

	auto counter = std::for_each(animals.begin(), animals.end(), DangCounter());

	std::cout << counter() << std::endl;

	auto animal = animals.find("lion");
	if (animal != animals.end()) {
		std::cout << (*animal)->refname() << std::endl;
	}

	auto range = animals.get<1>().equal_range(true);
	if (range.first != range.second) {
		std::for_each(range.first, range.second, [](std::shared_ptr<Animal> const &spAnimal){ std::cout << spAnimal->refname() << std::endl; });
		auto cnt = std::for_each(range.first, range.second, DangCounter());
		std::cout << cnt() << std::endl;
	}

	auto &index2 = animals.get<1>();
	auto animal2 = index2.find(2);
	if (animal2 != index2.end()) {
		std::cout << (*animal2)->refname() << std::endl;
	}

	return 0;
}
