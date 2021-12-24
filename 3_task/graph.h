#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stdexcept>
#include <memory>
#include <queue>

struct Comparator {
    bool operator()(const std::pair<char, char> &a, const std::pair<char, char> &b) const {
        if (a.first == b.first) {
            return a.second < b.second;
        }
        return a.first < b.first;
    }
};

using TVerticeContainer = std::set<char>;
using TConstVerticeContainer = const std::set<char>;

using TEdgeContainer = std::set<std::pair<char, char>, Comparator>;
using TConstEdgeContainer = const std::set<std::pair<char, char>, Comparator>;

using TWeightContainer = std::map<std::pair<char, char>, size_t, Comparator>;
using TConstWeightContainer = const std::map<std::pair<char, char>, size_t, Comparator>;

using TWayContainer = std::vector<std::pair<char, char>>;
using TConstWayContainer = const std::vector<std::pair<char, char>>;

class TOptions {
    TVerticeContainer vertices, additional_vertices;
    std::optional<size_t> weight;
    TEdgeContainer edges;
    TWeightContainer weights;

public:
    TOptions(TConstEdgeContainer &new_edges) : edges(new_edges) {};
    TOptions(TConstVerticeContainer &new_vertices) : vertices(new_vertices) {};
    TOptions(TConstVerticeContainer &new_vertices,
             TConstVerticeContainer &new_additional_vertices) :
            vertices(new_vertices),
            additional_vertices(new_additional_vertices) {};
    TOptions(size_t new_weight,
             TConstEdgeContainer &new_edges) :
            weight(new_weight),
            edges(new_edges) {};
    TOptions(TConstEdgeContainer &new_edges,
             TConstWeightContainer &new_weights) :
            edges(new_edges),
            weights(new_weights) {};
    TOptions(size_t new_weight,
             TConstEdgeContainer &new_edges,
             TConstWeightContainer &new_weights) :
            weight(new_weight),
            edges(new_edges),
            weights(new_weights) {};

    TVerticeContainer GetVertices() const {
        return vertices;
    }

    TVerticeContainer GetAdditionalVertices() const {
        return additional_vertices;
    }

    std::optional<size_t> GetWeight() const {
        return weight;
    }

    TEdgeContainer GetEdges() const {
        return edges;
    }

    TWeightContainer GetWeights() const {
        return weights;
    }
};

class TGraph {
public:
    virtual ~TGraph() = default;

    virtual const std::string ToString() const = 0;
    virtual const TConstVerticeContainer GetVertices() const = 0;
    virtual const TConstEdgeContainer GetEdges() const = 0;
};

using TGraphPtr = std::unique_ptr<TGraph>;

class TWeightedGraph : public TGraph {
    size_t default_weight;
    TEdgeContainer edges;
    TWeightContainer weights;

    void CheckInputGraph() const;

public:
    /*
    TWeightedGraph(TConstEdgeContainer& new_edges) :
                   default_weight(1),
                   edges(new_edges) {
        CheckInputGraph();
        for (auto& edge : edges) {
            weights[edge] = default_weight;
        }
    };
    TWeightedGraph(size_t new_weight,
                   TConstEdgeContainer& new_edges) :
                   default_weight(new_weight),
                   edges(new_edges) {
        CheckInputGraph();
        for (auto& edge : edges) {
            weights[edge] = default_weight;
        }
    };
    TWeightedGraph(TConstEdgeContainer& new_edges,
                   TConstWeightContainer& new_weights) :
                   default_weight(1),
                   edges(new_edges),
                   weights(new_weights) {
        CheckInputGraph();
        for (auto& edge : edges) {
            if (weights.find(edge) == weights.end()) {
                weights[edge] = default_weight;
            }
        }
    };
    TWeightedGraph(size_t new_weight,
                   TConstEdgeContainer& new_edges,
                   TConstWeightContainer& new_weights) :
                   default_weight(new_weight),
                   edges(new_edges),
                   weights(new_weights) {
        CheckInputGraph();
        for (auto& edge : edges) {
            if (weights.find(edge) == weights.end()) {
                weights[edge] = default_weight;
            }
        }
    };
    */
    TWeightedGraph(const TOptions& opts);

    const std::string ToString() const override;
    size_t GetDefaultWeight() const { return default_weight; };
    TConstVerticeContainer GetVertices() const override;
    TConstEdgeContainer GetEdges() const override { return edges; };
    TConstWeightContainer GetWeights() const { return weights; };

    TWayContainer GetShortestWay(char start, char finish) const;

    TWeightedGraph operator+(const TWeightedGraph& addedGraph);
    void operator+(const TGraph& addedGraph) { throw std::logic_error("Deprecated weighted + unweighted"); };
    TWeightedGraph& operator+=(const TWeightedGraph& addedGraph) { return *this = *this + addedGraph; };
    void operator+=(const TGraph& addedGraph) { throw std::logic_error("Deprecated weighted + unweighted"); };

    TWeightedGraph operator-(const TGraph& subtractedGraph);
    TWeightedGraph& operator-=(const TGraph& subtractedGraph) { return *this = *this - subtractedGraph; };
};

class TSimpleGraph : public TGraph {
    TEdgeContainer edges;

    void CheckInputGraph() const;

public:
    // TSimpleGraph(TConstEdgeContainer& new_edges) : edges(new_edges) { CheckInputGraph(); };
    TSimpleGraph(const TOptions& opts);

    const std::string ToString() const override;
    TConstVerticeContainer GetVertices() const override;
    TConstEdgeContainer GetEdges() const override { return edges; };

    TWeightedGraph AsWeighted(size_t default_weight) const { return TWeightedGraph(TOptions(default_weight, edges)); };

    void operator+(const TWeightedGraph& addedGraph) { throw std::logic_error("Deprecated simple + weighted"); };
    TSimpleGraph operator+(const TGraph& addedGraph);
    void operator+=(const TWeightedGraph& addedGraph) { throw std::logic_error("Deprecated simple + weighted"); };
    TSimpleGraph& operator+=(const TGraph& addedGraph) { return *this = *this + addedGraph; };

    TSimpleGraph operator-(const TGraph& subtractedGraph);
    TSimpleGraph& operator-=(const TGraph& subtractedGraph) { return *this = *this - subtractedGraph; };
};

class TBipartiteGraph : public TGraph {
    TVerticeContainer left_vertices, right_vertices;

    void CheckInputGraph() const;

public:
    /*
    TBipartiteGraph(TConstVerticeContainer& first,
                    TConstVerticeContainer& second) :
                    left_vertices(first),
                    right_vertices(second) {
        CheckInputGraph();
    };
     */
    TBipartiteGraph(const TOptions& opts);

    const std::string ToString() const override;
    TConstVerticeContainer GetVertices() const override;
    TConstEdgeContainer GetEdges() const override;

    TWeightedGraph AsWeighted(size_t default_weight) const { return TWeightedGraph(TOptions(default_weight, GetEdges())); };

    TBipartiteGraph operator+(const TBipartiteGraph& addedGraph);
    void operator+(const TWeightedGraph& addedGraph) { throw std::logic_error("Deprecated bipartite + weighted"); };
    TSimpleGraph operator+(const TGraph& addedGraph);
    TBipartiteGraph& operator+=(const TBipartiteGraph& addedGraph) { return *this = *this + addedGraph; };
    void operator+=(const TWeightedGraph& addedGraph) { throw std::logic_error("Deprecated bipartite + weighted"); };

    TBipartiteGraph operator-(const TBipartiteGraph& subtractedGraph);
    TSimpleGraph operator-(const TGraph& subtractedGraph);
    TBipartiteGraph& operator-=(const TBipartiteGraph& subtractedGraph) { return *this = *this - subtractedGraph; };
};

class TCompleteGraph : public TGraph {
    TVerticeContainer vertices;

public:
    // TCompleteGraph(TConstVerticeContainer& new_vertices) : vertices(new_vertices) {};
    TCompleteGraph(const TOptions& opts);

    const std::string ToString() const override;
    TConstVerticeContainer GetVertices() const override { return vertices; };
    TConstEdgeContainer GetEdges() const override;

    TWeightedGraph AsWeighted(size_t default_weight) const { return TWeightedGraph(TOptions(default_weight, GetEdges())); };

    TCompleteGraph operator+(const TCompleteGraph& addedGraph);
    void operator+(const TWeightedGraph& addedGraph) { throw std::logic_error("Deprecated complete + weighted"); };
    TSimpleGraph operator+(const TGraph& addedGraph);
    TCompleteGraph& operator+=(const TCompleteGraph& addedGraph) { return *this = *this + addedGraph; };
    void operator+=(const TWeightedGraph& addedGraph) { throw std::logic_error("Deprecated complete + weighted"); };

    TCompleteGraph operator-(const TCompleteGraph& subtractedGraph);
    TSimpleGraph operator-(const TGraph& subtractedGraph);
    TCompleteGraph& operator-=(const TCompleteGraph& subtractedGraph) { return *this = *this - subtractedGraph; };
};

using TWeightedGraphPtr = std::unique_ptr<TWeightedGraph>;
using TSimpleGraphPtr = std::unique_ptr<TSimpleGraph>;
using TBipartiteGraphPtr = std::unique_ptr<TBipartiteGraph>;
using TCompleteGraphPtr = std::unique_ptr<TCompleteGraph>;
