#pragma once
#include <iostream>
#include <random>
#include <vector>
#include "graph.hpp"

constexpr float kGreenProbabilty = 0.1, kRedProbabilty = 0.33;
float getGreyProbability(float step, int depth) {
  return 1.0 - step * depth;
}
float getYellowProbability(const uni_course_cpp::Graph& graph,
                           const uni_course_cpp::VertexId& vertex_id) {
  return 1.0 * graph.vertexDepth(vertex_id) /
         (graph.depth() - 1);  // The probabilty of generating a yellow edge is
                               // (current depth/overall depth-1)
}
bool randomValue(float probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution distribution(probability);
  return distribution(gen);
}
int getRandomVertexId(const std::vector<uni_course_cpp::VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> random_number(0, vertex_ids.size() - 1);
  return vertex_ids.at(random_number(gen));
}

class GraphGenerator {
 public:
  struct Params {
    explicit Params(int _depth = 0, int _new_vertexes_num = 0)
        : depth(_depth), new_vertexes_num(_new_vertexes_num) {}

    uni_course_cpp::Graph::Depth depth;
    const int new_vertexes_num;
  };

  explicit GraphGenerator(const Params& params = Params()) : params_(params) {}
  uni_course_cpp::Graph generate() const {
    uni_course_cpp::Graph graph = generateMainBody();
    generateColorEdges(graph);
    return graph;
  }

 private:
  uni_course_cpp::Graph generateMainBody() const {
    uni_course_cpp::Graph graph;
    const float step = 1.0 / params_.depth;
    graph.addVertex();
    for (uni_course_cpp::Graph::Depth current_depth = 0;
         current_depth < params_.depth; current_depth++) {
      bool vertexes_generated = false;
      for (const auto& vertex_id : graph.vertexIdsAtLayer(current_depth)) {
        for (int j = 0; j < params_.new_vertexes_num; j++) {
          if (randomValue(getGreyProbability(step, current_depth))) {
            graph.addEdge(vertex_id, graph.addVertex());
            vertexes_generated = true;
          }
        }
      }
      if (!vertexes_generated) {
        break;
      }
    }
    if (graph.depth() < params_.depth) {
      std::cout << "Depth of the graph is less than given. Depth is "
                << graph.depth() << std::endl;
    }
    return graph;
  }
  void generateColorEdges(uni_course_cpp::Graph& graph) const {
    for (const auto& vertex : graph.vertexes()) {
      const int vertex_depth = graph.vertexDepth(vertex.id);
      if (randomValue(kGreenProbabilty)) {
        graph.addEdge(vertex.id, vertex.id);
      }
      if (vertex_depth < graph.depth() &&
          randomValue(getYellowProbability(graph, vertex.id))) {
        std::vector<uni_course_cpp::VertexId> next_layer;
        for (const auto& vertex_id : graph.vertexIdsAtLayer(
                 vertex_depth + 1))  // find vertex from next layer
        {
          if (!graph.isConnected(vertex.id, vertex_id)) {
            next_layer.push_back(vertex_id);
          }
        }
        if (next_layer.size() != 0)
          graph.addEdge(vertex.id, getRandomVertexId(next_layer));
      }
      if (randomValue(kRedProbabilty) &&
          vertex_depth < (graph.depth() - 1))  // depth(N-1){
        graph.addEdge(vertex.id, getRandomVertexId(graph.vertexIdsAtLayer(
                                     vertex_depth +
                                     2)));  // fin vertex from next next layer
    }
  }
  const GraphGenerator::Params params_ = Params();
};