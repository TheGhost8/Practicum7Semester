#include "graph.h"


/// TWeightedGraph

void TWeightedGraph::CheckInputGraph() const {
    TEdgeContainer copy_edges = edges;

    while (copy_edges.size() > 1) {
        if (copy_edges.find(std::make_pair(copy_edges.begin()->second, copy_edges.begin()->first)) != copy_edges.end()) {
            throw std::logic_error("Invalid arguments for weighted graph");
        }
        copy_edges.erase(copy_edges.begin());
    }
    if ((copy_edges.size() == 1) && (copy_edges.begin()->first == copy_edges.begin()->second)) {
        throw std::logic_error("Invalid arguments for weighted graph");
    }
}

TWeightedGraph::TWeightedGraph(const TOptions &opts) {
    edges = opts.GetEdges();
    CheckInputGraph();

    if (opts.GetWeight()) {
        default_weight = opts.GetWeight().value();
    }
    for (auto &w : opts.GetWeights()) {
        weights.insert(w);
    }

    for (auto& edge : edges) {
        if (weights.find(edge) == weights.end()) {
            weights[edge] = default_weight;
        }
    }
}

const std::string TWeightedGraph::ToString() const {
    std::string stringView = "weighted {";

    for (auto& edge : edges) {
        stringView += std::string(1, edge.first) + std::string(1, edge.second);
        stringView += ": " + std::to_string((weights.find(edge) == weights.end()) ? default_weight : weights.at(edge)) + ", ";
    }
    stringView.erase(stringView.size() - 2);
    return stringView + "}";
}

TConstVerticeContainer TWeightedGraph::GetVertices() const {
    TVerticeContainer vertices;

    for (auto &edge : edges) {
        vertices.insert(edge.first);
        vertices.insert(edge.second);
    }

    return vertices;
}

TWayContainer TWeightedGraph::GetShortestWay(char start, char finish) const {
    TVerticeContainer visited_nodes;
    std::queue<char> nodes;
    nodes.push(start);
    std::map<char, TWayContainer> ways;
    std::map<char, size_t> ways_length;
    for (auto &v : GetVertices()) {
        if (v != start) {
            ways_length[v] = -1;
        }
        else {
            ways_length[v] = 0;
        }
    }
    ways[start] = TWayContainer();

    while (!nodes.empty()) {
        for (auto &edge : edges) {
            if ((edge.first == nodes.front()) && (visited_nodes.find(edge.second) == visited_nodes.end())) {
                nodes.push(edge.second);
            }
            else if ((edge.first == nodes.front()) && (visited_nodes.find(edge.second) != visited_nodes.end())) {
                if (ways_length[edge.second] + weights.at(edge) < ways_length[edge.first]) {
                    ways[edge.first] = ways[edge.second];
                    ways[edge.first].push_back(edge);
                    ways_length[edge.first] = ways_length[edge.second] + weights.at(edge);
                }
            }
            else if ((edge.second == nodes.front()) && (visited_nodes.find(edge.first) == visited_nodes.end())) {
                nodes.push(edge.first);
            }
            else if ((edge.second == nodes.front()) && (visited_nodes.find(edge.first) != visited_nodes.end())) {
                if (ways_length[edge.first] + weights.at(edge) < ways_length[edge.second]) {
                    ways[edge.second] = ways[edge.first];
                    ways[edge.second].push_back(edge);
                    ways_length[edge.second] = ways_length[edge.first] + weights.at(edge);
                }
            }
        }

        visited_nodes.insert(nodes.front());
        nodes.pop();
    }

    return ways[finish];
}

TWeightedGraph TWeightedGraph::operator+(const TWeightedGraph &addedGraph) {
    default_weight = ((default_weight < addedGraph.default_weight) ? default_weight : addedGraph.default_weight);

    for (auto &edge : addedGraph.edges) {
        if (edges.find(edge) == edges.end()) {
            edges.insert(edge);
            weights[edge] = addedGraph.weights.at(edge);
        }
        else {
            weights[edge] = ((weights[edge] < addedGraph.weights.at(edge)) ? weights[edge] : addedGraph.weights.at(edge));
        }
    }
}

TWeightedGraph TWeightedGraph::operator-(const TGraph &subtractedGraph) {
    auto &subtractedEdges = subtractedGraph.GetEdges();

    for (auto &edge : subtractedEdges) {
        if (edges.find(edge) != edges.end()) {
            edges.erase(edge);
            weights.erase(edge);
        }
    }
}


/// TSimpleGraph

void TSimpleGraph::CheckInputGraph() const {
    TEdgeContainer copy_edges = edges;

    while (copy_edges.size() > 1) {
        if (copy_edges.find(std::make_pair(copy_edges.begin()->second, copy_edges.begin()->first)) != copy_edges.end()) {
            throw std::logic_error("Invalid arguments for simple graph");
        }
        copy_edges.erase(copy_edges.begin());
    }
    if ((copy_edges.size() == 1) && (copy_edges.begin()->first == copy_edges.begin()->second)) {
        throw std::logic_error("Invalid arguments for simple graph");
    }
}

TSimpleGraph::TSimpleGraph(const TOptions &opts) {
    edges = opts.GetEdges();
    CheckInputGraph();
}

const std::string TSimpleGraph::ToString() const {
    std::string stringView = "simple {";

    for (auto& edge : edges) {
        stringView += std::string(1, edge.first) + std::string(1, edge.second);
        //stringView.push_back(edge.first);
        //stringView.push_back(edge.second);
        stringView += ", ";
    }
    stringView.erase(stringView.size() - 2);
    return stringView + "}";
}

TConstVerticeContainer TSimpleGraph::GetVertices() const {
    TVerticeContainer vertices;

    for (auto &edge : edges) {
        vertices.insert(edge.first);
        vertices.insert(edge.second);
    }

    return vertices;
}

TSimpleGraph TSimpleGraph::operator+(const TGraph &addedGraph) {
    auto &addedEdges = addedGraph.GetEdges();

    for (auto &edge : addedEdges) {
        if ((edges.find(edge) == edges.end()) && (edges.find(std::make_pair(edge.second, edge.first)) == edges.end())) {
            edges.insert(edge);
        }
    }
}

TSimpleGraph TSimpleGraph::operator-(const TGraph &subtractedGraph) {
    auto &subtractedEdges = subtractedGraph.GetEdges();

    for (auto &edge : subtractedEdges) {
        if (edges.find(edge) != edges.end()) {
            edges.erase(edge);
        }
        else if (edges.find(std::make_pair(edge.second, edge.first)) != edges.end()) {
            edges.erase(std::make_pair(edge.second, edge.first));
        }
    }
}


/// TBipartiteGraph

void TBipartiteGraph::CheckInputGraph() const {
    for (auto &l_v : left_vertices) {
        if (right_vertices.find(l_v) != right_vertices.end()) {
            throw std::logic_error("Invalid arguments for bipartite graph");
        }
    }
}

TBipartiteGraph::TBipartiteGraph(const TOptions &opts) {
    left_vertices = opts.GetVertices();
    right_vertices = opts.GetAdditionalVertices();
    CheckInputGraph();
}

const std::string TBipartiteGraph::ToString() const {
    std::string stringView = "simple {{";

    for (auto& l_v : left_vertices) {
        stringView += std::string(1, l_v) + ", ";
    }
    stringView.erase(stringView.size() - 2);
    stringView += "}, {";
    for (auto& r_v : right_vertices) {
        stringView += std::string(1, r_v) + ", ";
        stringView += ", ";
    }
    stringView.erase(stringView.size() - 2);
    return stringView + "}}";
}

TConstVerticeContainer TBipartiteGraph::GetVertices() const {
    TVerticeContainer vertices;

    for (auto &l_v : left_vertices) {
        vertices.insert(l_v);
    }
    for (auto &r_v : right_vertices) {
        vertices.insert(r_v);
    }

    return vertices;
}

TConstEdgeContainer TBipartiteGraph::GetEdges() const {
    TEdgeContainer edges;

    for (auto &l_v : left_vertices) {
        for (auto &r_v : right_vertices) {
            edges.insert(std::make_pair(l_v, r_v));
        }
    }

    return edges;
}

TBipartiteGraph TBipartiteGraph::operator+(const TBipartiteGraph &addedGraph) {
    for (auto &l_v : addedGraph.left_vertices) {
        left_vertices.insert(l_v);
    }
    for (auto &r_v : addedGraph.right_vertices) {
        right_vertices.insert(r_v);
    }

    try {
        CheckInputGraph();
    }
    catch (std::logic_error& err) {
        throw std::logic_error("Invalid graphs for sum");
    }
}

TSimpleGraph TBipartiteGraph::operator+(const TGraph &addedGraph) {
    TEdgeContainer sum_edges = addedGraph.GetEdges();
    for (auto &edge : GetEdges()) {
        sum_edges.insert(edge);
    }
    return TSimpleGraph(TOptions(sum_edges));
}

TBipartiteGraph TBipartiteGraph::operator-(const TBipartiteGraph &subtractedGraph) {
    for (auto &l_v : subtractedGraph.left_vertices) {
        left_vertices.erase(l_v);
    }
    for (auto &r_v : subtractedGraph.right_vertices) {
        right_vertices.erase(r_v);
    }
}

TSimpleGraph TBipartiteGraph::operator-(const TGraph &subtractedGraph) {
    TEdgeContainer subtracted_edges = GetEdges();
    for (auto &edge : subtractedGraph.GetEdges()) {
        subtracted_edges.erase(edge);
    }
    return TSimpleGraph(TOptions(subtracted_edges));
}


/// TCompleteGraph

TCompleteGraph::TCompleteGraph(const TOptions &opts) {
    vertices = opts.GetVertices();
}

const std::string TCompleteGraph::ToString() const {
    std::string stringView = "complete {";

    for (auto &v : vertices) {
        stringView += std::string(1, v) + ", ";
    }
    stringView.erase(stringView.size() - 2);
    return stringView + "}";
}

TConstEdgeContainer TCompleteGraph::GetEdges() const {
    TEdgeContainer edges;

    for (auto &v1 : vertices) {
        for (auto &v2 : vertices) {
            if ((v1 != v2) && (edges.find(std::make_pair(v2, v1)) == edges.end())) {
                edges.insert(std::make_pair(v1, v2));
            }
        }
    }

    return edges;
}

TCompleteGraph TCompleteGraph::operator+(const TCompleteGraph &addedGraph) {
    //std::cout << "check\n";
    for (auto &v : addedGraph.vertices) {
        vertices.insert(v);
    }
    return *this;
}

TSimpleGraph TCompleteGraph::operator+(const TGraph &addedGraph) {
    TEdgeContainer sum_edges = addedGraph.GetEdges();
    for (auto &edge : GetEdges()) {
        sum_edges.insert(edge);
    }
    return TSimpleGraph(TOptions(sum_edges));
}

TCompleteGraph TCompleteGraph::operator-(const TCompleteGraph &subtractedGraph) {
    for (auto &v : subtractedGraph.vertices) {
        vertices.erase(v);
    }
}

TSimpleGraph TCompleteGraph::operator-(const TGraph &subtractedGraph) {
    TEdgeContainer subtracted_edges = GetEdges();
    for (auto &edge : subtractedGraph.GetEdges()) {
        subtracted_edges.erase(edge);
    }
    return TSimpleGraph(TOptions(subtracted_edges));
}