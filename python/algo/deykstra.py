def __find_lowest_cost_node__(costs, processed):
    lowest_cost = float("inf")
    lowest_cost_node = None
    for node, cost in costs.items():
        if cost < lowest_cost and node not in processed:
            lowest_cost = cost
            lowest_cost_node = node
    return lowest_cost_node


def deykstra(graph, costs, parents):
    processed = []

    node = __find_lowest_cost_node__(costs, processed)
    while node is not None:
        cost = costs[node]
        neighs = graph[node]
        for n, c in neighs.items():
            new_cost = cost + c
            if new_cost < costs[n]:
                costs[n] = new_cost
                parents[n] = node
        processed.append(node)
        node = __find_lowest_cost_node__(costs, processed)

    print(costs)
    print(parents)


if __name__ == "__main__":
    graph = {}

    graph["start"] = {}
    graph["start"]["a"] = 6
    graph["start"]["b"] = 2

    graph["a"] = {}
    graph["a"]["fin"] = 1

    graph["b"] = {}
    graph["b"]["a"] = 3
    graph["b"]["fin"] = 5

    graph["fin"] = {}

    infinity = float("inf")

    costs = {}
    costs["a"] = 6
    costs["b"] = 2
    costs["fin"] = infinity

    parents = {}
    parents["a"] = "start"
    parents["b"] = "start"
    parents["fin"] = None

    deykstra(graph, costs, parents)
