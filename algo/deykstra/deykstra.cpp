#include <limits>
#include <print>
#include <unordered_set>
#include <vector>

struct neigh {
  int id;
  int weight;
};

struct node {
  int id;
  std::vector<neigh> neighs;
};

using node_t = node;
using graph_t = std::vector<node_t>;
using costs_t = std::vector<int>;
using processed_t = std::unordered_set<int>;
using parents_t = std::vector<int>;

node_t const *find_lowest_cost_node(graph_t const &graph, costs_t const &costs,
                                    processed_t const &processed) {
  int lowest_cost_node = std::numeric_limits<int>::max();
  node_t const *node = nullptr;
  for (int i = 0; i < costs.size(); ++i) {
    if (!processed.contains(i) && costs[i] < lowest_cost_node) {
      node = &graph[i];
      lowest_cost_node = costs[i];
    }
  }
  return node;
}

void deykstra(graph_t const &graph, costs_t &costs, parents_t &parents) {
  processed_t processed;

  while (auto const *node = find_lowest_cost_node(graph, costs, processed)) {
    auto const cost = costs[node->id];
    for (auto const &neigh : node->neighs) {
      if (auto const new_cost = cost + neigh.weight;
          new_cost < costs[neigh.id]) {
        costs[neigh.id] = new_cost;
        parents[neigh.id] = node->id;
      }
    }
    processed.insert(node->id);
  }
}

int main(int argc, char const *argv[]) {
  graph_t graph;

  graph.push_back({0, {{1, 6}, {2, 2}}});
  graph.push_back({1, {{3, 1}}});
  graph.push_back({2, {{1, 3}, {3, 5}}});
  graph.push_back({3, {}});

  costs_t costs(graph.size(), std::numeric_limits<int>::max());
  parents_t parents(graph.size(), -1);

  for (auto const &neigh : graph[0].neighs) {
    costs[neigh.id] = neigh.weight;
    parents[neigh.id] = graph[0].id;
  }

  deykstra(graph, costs, parents);

  for (int i = 0; i < parents.size(); ++i)
    std::println("{} -> {}", i, parents[i]);
  std::println("");

  for (int i = 0; i < parents.size(); ++i)
    std::println("{} -> {}", i, costs[i]);
  std::println("");

  return 0;
}
