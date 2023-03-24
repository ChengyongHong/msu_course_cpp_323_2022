#include "graph.hpp"
#include "graph_generator.hpp"

#include <fstream>
#include <iostream>
#include <string>

class GraphPrinter {
 public:
  explicit GraphPrinter(const uni_course_cpp::Graph& graph) : graph_(graph) {}
  std::string print() const {
    std::string graph_string;
    graph_string += "{\n \"vertices\": [\n  ";
    for (const auto& vertex : graph_.vertexes()) {
      graph_string += printVertex(vertex.id);
    }
    if (graph_.vertexes().size() > 0) {
      graph_string.pop_back();
      graph_string.pop_back();
    }
    graph_string += "\n ],\n \"edges\": [\n  ";
    for (const auto& edge : graph_.edges()) {
      graph_string += printEdge(edge);
    }
    if (graph_.edges().size() > 0) {
      graph_string.pop_back();
      graph_string.pop_back();
    }
    graph_string += "\n ]\n}\n";
    return graph_string;
  }
  std::string printVertex(const uni_course_cpp::VertexId& id) const {
    std::string vertex_string =
        "{\n   \"id\": " + std::to_string(id) + ",\n   \"edge_ids\": [";
    for (const auto& edge_id : graph_.vertexConnections(id)) {
      vertex_string += std::to_string(edge_id) + ", ";
    }
    if (graph_.vertexConnections(id).size() > 0) {
      vertex_string.pop_back();
      vertex_string.pop_back();
    }
    vertex_string +=
        "],\n   \"depth\": " + std::to_string(graph_.vertexDepth(id)) +
        "\n  }, ";
    return vertex_string;
  }
  std::string printEdge(const uni_course_cpp::Edge& edge) const {
    std::string edge_string = "{\n   \"id\": " + std::to_string(edge.id);
    edge_string += ",\n   \"vertex_ids\": [";
    edge_string += std::to_string(edge.from_vertex_id) + ", ";
    edge_string += std::to_string(edge.to_vertex_id) + "],\n   ";
    edge_string += "\"color\": " + colorToString(edge) + "\n  }, ";
    return edge_string;
  }

 private:
  const uni_course_cpp::Graph& graph_;
  std::string colorToString(const uni_course_cpp::Edge& edge) const {
    switch (edge.color) {
      case uni_course_cpp::Edge::Colors::Grey:
        return "\"gray\"";
      case uni_course_cpp::Edge::Colors::Green:
        return "\"green\"";
      case uni_course_cpp::Edge::Colors::Blue:
        return "\"blue\"";
      case uni_course_cpp::Edge::Colors::Yellow:
        return "\"yellow\"";
      case uni_course_cpp::Edge::Colors::Red:
        return "\"red\"";
    }
  }
};

void write_to_file(const std::string& string, const std::string& file_name) {
  std::ofstream file(file_name);
  file << string;
  file.close();
}

int main() {
  uni_course_cpp::Depth depth, new_vertexes_num;
  std::cout << "Enter depth:" << std::endl;
  std::cin >> depth;
  while (depth < 0) {
    std::cout << "Depth must be not negative. Enter depth again:" << std::endl;
    std::cin >> depth;
  }
  std::cout << "Enter number of new vertices:" << std::endl;
  std::cin >> new_vertexes_num;
  while (depth < 0) {
    std::cout << "Number of new vertices must be not negative. Enter number of "
                 "new vertexes again:"
              << std::endl;
    std::cin >> new_vertexes_num;
  }
  const uni_course_cpp::GraphGenerator::Params params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertexes_num);
  const uni_course_cpp::Graph graph =
      uni_course_cpp::GraphGenerator(params).generate();
  const auto graph_printer = GraphPrinter(graph);
  const auto graph_json = graph_printer.print();
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");
  return 0;
}
