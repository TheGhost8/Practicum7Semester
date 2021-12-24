#include <iostream>
#include <gtest/gtest.h>

#include "graph.h"
#include "factory.h"

using TVerticeContainer = std::set<char>;
using TConstVerticeContainer = const std::set<char>;

using TEdgeContainer = std::set<std::pair<char, char>, Comparator>;
using TConstEdgeContainer = const std::set<std::pair<char, char>, Comparator>;

using TWeightContainer = std::map<std::pair<char, char>, size_t, Comparator>;
using TConstWeightContainer = const std::map<std::pair<char, char>, size_t, Comparator>;

using TGraphPtr = std::unique_ptr<TGraph>;
using TWeightedGraphPtr = std::unique_ptr<TWeightedGraph>;
using TSimpleGraphPtr = std::unique_ptr<TSimpleGraph>;
using TBipartiteGraphPtr = std::unique_ptr<TBipartiteGraph>;
using TCompleteGraphPtr = std::unique_ptr<TCompleteGraph>;

/*
TEST(HelloTest, BasicAssertions) {
    //EXPECT_STRNE("hello", "world");
    //EXPECT_EQ(7 * 6, 42);
}
*/

int main() {
    TFactory hFactory;
    TOptions opts({{'A', 'B'}, {'C', 'D'}, {'B', 'C'}});
    //TOptions opts({{'A', 'B'}, {'B', 'C'}, {'C', 'D'}, {'A', 'D'}}, {{{'A', 'D'}, 2}});
    TOptions opts2({'A', 'B', 'C'});
    TOptions opts3({'D', 'E', 'C'});
    TWeightedGraphPtr pWeightedGraph(dynamic_cast<TWeightedGraph*>(hFactory.CreateGraph("weighted", opts).release()));
    TCompleteGraphPtr pCompleteGraph(dynamic_cast<TCompleteGraph*>(hFactory.CreateGraph("complete", opts2).release()));
    TCompleteGraphPtr pCompleteGraph3(dynamic_cast<TCompleteGraph*>(hFactory.CreateGraph("complete", opts3).release()));
    std::vector<std::string> availableGraphs = hFactory.GetAvailableObjects();

    /*
    for (auto &w : pWeightedGraph->GetWeights()) {
        std::cout << std::string(1, w.first.first) << std::string(1, w.first.second) << ": " <<std::to_string(w.second) << std::endl;
    }
    */

    for (auto& p : pWeightedGraph->GetShortestWay('A', 'D')) {
        std::cout << std::string(1, p.first) << std::string(1, p.second) << "->";
    }
    std::cout << std::endl;

    std::cout << pCompleteGraph->ToString() << std::endl;
    *pCompleteGraph += *pCompleteGraph3;
    std::cout << pCompleteGraph->ToString() << std::endl;

    return 0;
}
