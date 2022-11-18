#include "graph.hpp"
#include <algorithm>
#include <vector>
#include "graph_json_printing.hpp"

namespace uni_course_cpp {

void Graph::add_vertex() {
  Graph::VertexId new_vertex_id = get_new_vertex_id_();
  vertices_.emplace_back(
      new_vertex_id);  // why we use emplace_back ?, (difficult to understand
                       // what is mean vertices)
  set_vertex_depth(new_vertex_id, 1);
  adjacency_list_[new_vertex_id] = {};
  adjacency_list_of_edges_[new_vertex_id] = {};
  graph_depth_ = std::max(graph_depth_, 1);
}

void Graph::set_vertex_depth(Graph::VertexId vertex_id, Depth depth) {
  vertices_.at(vertex_id).set_depth(depth);
  graph_depth_ = std::max(graph_depth_, depth);

  auto index = std::find(depth_to_vertices_[1].begin(),
                         depth_to_vertices_[1].end(), vertices_.at(vertex_id));

  if (index != depth_to_vertices_[1].end()) {
    depth_to_vertices_[1].erase(index);
  }
  depth_to_vertices_[depth].emplace_back(vertex_id);
}

bool Graph::is_connected(Graph::VertexId from_vertex_id,
                         Graph::VertexId to_vertex_id) const {
  return std::count(adjacency_list_.at(from_vertex_id).begin(),
                    adjacency_list_.at(from_vertex_id).end(), to_vertex_id);
}

const std::vector<Graph::EdgeId>& Graph::connected_edges_ids(
    Graph::VertexId vertex_id) const {
  if (adjacency_list_of_edges_.count(vertex_id) == 0) {
    const std::vector<Graph::EdgeId> empty_edges_list;
    return std::move(empty_edges_list);
  }
  return adjacency_list_of_edges_.at(vertex_id);
}

Graph::Edge::Color Graph::get_edge_color(Graph::VertexId from_vertex_id,
                                         Graph::VertexId to_vertex_id) const {
  const auto from_vertex_depth =
      vertices_.at(from_vertex_id)
          .depth();  // runtime error if Graph::Edge not in graph
  const auto to_vertex_depth =
      vertices_.at(to_vertex_id)
          .depth();  // runtime error if Graph::Edge not in graph

  if (from_vertex_id == to_vertex_id) {
    return Graph::Edge::Color::Green;
  }
  if (adjacency_list_.at(to_vertex_id).size() == 0) {
    return Graph::Edge::Color::Grey;
  }
  if (to_vertex_depth - from_vertex_depth == 1 &&
      !is_connected(from_vertex_id, to_vertex_id)) {
    return Graph::Edge::Color::Yellow;
  }
  if (to_vertex_depth - from_vertex_depth == 2) {
    return Graph::Edge::Color::Red;
  }
  throw std::runtime_error("Failed to determine color");
}

void Graph::add_edge(Graph::VertexId from_vertex_id,
                     Graph::VertexId to_vertex_id) {
  const auto color = get_edge_color(
      from_vertex_id,
      to_vertex_id);  // runtime error if Graph::Edge not in graph

  Graph::EdgeId new_id = get_new_edge_id_();
  if (color == Graph::Edge::Color::Grey) {
    set_vertex_depth(to_vertex_id, vertices_[from_vertex_id].depth() + 1);
    adjacency_list_[from_vertex_id].emplace_back(to_vertex_id);
    adjacency_list_[to_vertex_id].emplace_back(from_vertex_id);
  }
  Edge edge(new_id, from_vertex_id, to_vertex_id, color);
  color_to_edges_[color].emplace_back(edge);
  adjacency_list_of_edges_[from_vertex_id].emplace_back(new_id);
  adjacency_list_of_edges_[to_vertex_id].emplace_back(new_id);
  edges_.emplace_back(edge);
}

std::vector<Graph::Vertex> Graph::get_vertices_with_depth(Depth depth) const {
  if (depth_to_vertices_.find(depth) != depth_to_vertices_.end()) {
    return depth_to_vertices_.at(depth);
  } else {
    return {};
  }
}

std::vector<Graph::Edge> Graph::get_edges_with_color(Edge::Color color) const {
  if (color_to_edges_.find(color) != color_to_edges_.end()) {
    return color_to_edges_.at(color);
  } else {
    return {};
  }
}

}  // namespace uni_course_cpp