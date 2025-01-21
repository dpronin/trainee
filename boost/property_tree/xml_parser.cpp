#include <algorithm>
#include <string>
#include <utility>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>

struct tag_info {
	int id = -1;
	std::string name;
	bool is_valid() const { return -1 != id && !name.empty(); }

	static std::vector<std::pair<std::string, std::function<void(std::string&&)>>> get_parser(tag_info &info) {
		return {
			{"id",   [&info](std::string &&attr_value) { info.id = boost::lexical_cast<int>(attr_value); }},
			{"name", [&info](std::string &&attr_value) { info.name = std::move(attr_value); }}
		};
	}
};

template<typename PropertyTreeAttrs>
tag_info parse_info(PropertyTreeAttrs const &tree_attrs) {
	tag_info info;
	auto parser_attrs = tag_info::get_parser(info);
	for (auto tree_it = tree_attrs.begin(); tree_it != tree_attrs.end() && !parser_attrs.empty(); ++tree_it) {
		auto it = std::find_if(parser_attrs.begin(), parser_attrs.end(), [tree_it](auto &&attr){
			return tree_it->first == attr.first;
		});
		if (parser_attrs.end() != it) {
			it->second(tree_it->second.template get_value<std::string>());
			*it = std::move(parser_attrs.back());
			parser_attrs.pop_back();
		} else {
			std::cout << "skipped unknown attribute '" << tree_it->first << "'" << std::endl;
		}
	}
	return info;
}

int main(int argc, char const *argv[]) {
	boost::property_tree::ptree ptree;
	boost::property_tree::xml_parser::read_xml("boost_property_tree/myxml.xml", ptree);
	auto item = ptree.get_child("");

	std::vector<tag_info> tags_info;
	for (auto &&tag : item) {
		auto info = parse_info(tag.second.get_child("<xmlattr>"));
		if (info.is_valid()) tags_info.push_back(std::move(info));
	}

	for (auto &&item : tags_info) {
		std::cout << "tag: id = " << item.id << ", name = " << item.name << std::endl;
	}
	return 0;
}
