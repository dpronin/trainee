#include <array>
#include <algorithm>
#include <ostream>
#include <iostream>
#include <iterator>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <boost/range/algorithm/copy.hpp>

#include <boost/core/ignore_unused.hpp>

struct null_visitor {
    typedef boost::on_no_event event_filter;
    template<typename T, typename Graph>
    void operator()(T, Graph&) const noexcept {}
};

struct discover_visitor {
    typedef boost::on_discover_vertex event_filter;
    template<typename T, typename U>
    void operator()(T&& t, U&& u) const noexcept {
        boost::ignore_unused(u);
        std::cout << t << std::endl;
    }
};

int main(int argc, char const *argv[]) {
    boost::ignore_unused(argc);
    boost::ignore_unused(argv);
    // boost::adjacency_list<> g;

    // boost::adjacency_list<>::vertex_descriptor v1 = boost::add_vertex(g);
    // boost::adjacency_list<>::vertex_descriptor v2 = boost::add_vertex(g);
    // boost::adjacency_list<>::vertex_descriptor v3 = boost::add_vertex(g);
    // boost::adjacency_list<>::vertex_descriptor v4 = boost::add_vertex(g);

    // auto p = boost::vertices(g);
    // for (auto it = p.first; it != p.second; ++it) {
    //  std::cout << *it << std::endl;
    // }

    // boost::add_edge(v1, v2, g);
    // boost::add_edge(v2, v3, g);
    // boost::add_edge(v3, v4, g);
    // boost::add_edge(v4, v1, g);

    // auto p1 = boost::edges(g);
    // for (auto it = p1.first; it != p1.second; ++it) {
    //  std::cout << *it << std::endl;
    // }

    using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int>>;
    using weight_t = boost::property<boost::edge_weight_t, int>;
    using enum_t = enum { topLeft, topRight, bottomRight, bottomLeft };

    graph_t g;
    add_edge(enum_t::topLeft, enum_t::topRight, weight_t(2), g);
    add_edge(enum_t::topRight, enum_t::bottomRight, weight_t(3), g);
    add_edge(enum_t::bottomRight, enum_t::bottomLeft, weight_t(1), g);
    add_edge(enum_t::bottomLeft, enum_t::topLeft, weight_t(1), g);

    // auto p2 = boost::vertices(g);
    // for (auto it = p2.first; it != p2.second; ++it) {
    //  std::cout << *it << std::endl;
    // }

    // auto p3 = boost::edges(g);
    // for (auto it = p3.first; it != p3.second; ++it) {
    //  std::cout << *it << std::endl;
    // }

    // boost::breadth_first_search(g, topLeft, boost::visitor(boost::make_bfs_visitor(null_visitor())));
    // boost::depth_first_search(g, boost::visitor(boost::make_dfs_visitor(discover_visitor())));

    std::array<int, 4> predecessors = { 0 };
    boost::dijkstra_shortest_paths(g, bottomRight, boost::predecessor_map(predecessors.data()));

    boost::copy(predecessors.begin(), predecessors.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    return 0;
}
