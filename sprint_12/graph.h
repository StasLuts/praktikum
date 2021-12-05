#pragma once

#include <optional>
#include "ranges.h"

#include <cstdlib>
#include <vector>

namespace graph
{

using VertexId = size_t;
using EdgeId = size_t;

enum class EdgeType
{
    WAIT,
    BUS
};

template <typename Weight>
struct Edge // ребро
{
    VertexId from; // остановка
    VertexId to; // остановка
    Weight weight; // время
    std::string_view bus_or_stop_name;
    EdgeType type;
    std::optional<size_t> span_count;
};

template <typename Weight>
class DirectedWeightedGraph // направленный взвешенный граф
{
private:

    // список событий
    using IncidenceList = std::vector<EdgeId>; // вектор номеров ребер
    // дивпазон ребер инцедента
    using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>; // хуйня списков событий

public:

    DirectedWeightedGraph() = default;

    // конструктор принимающий колличество  вершин
    explicit DirectedWeightedGraph(size_t vertex_count);

    // добавить ребро
    EdgeId AddEdge(const Edge<Weight>& edge);

    // вернуть колличество вершин
    size_t GetVertexCount() const;

    // вернуть колличество ребер
    size_t GetEdgeCount() const;

    // вернуть ребро по ай-ди ребра
    const Edge<Weight>& GetEdge(EdgeId edge_id) const;

    // вернуть ребра инцедента
    IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

private:

    std::vector<Edge<Weight>> edges_; // ребра
    std::vector<IncidenceList> incidence_lists_; // события
};

// определения 

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
    : incidence_lists_(vertex_count) {}

template <typename Weight>
EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge)
{
    edges_.push_back(edge);
    const EdgeId id = edges_.size() - 1;
    incidence_lists_.at(edge.from).push_back(id);
    return id;
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetVertexCount() const
{
    return incidence_lists_.size();
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const
{
    return edges_.size();
}

template <typename Weight>
const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const
{
    return edges_.at(edge_id);
}

template <typename Weight>
typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const
{
    return ranges::AsRange(incidence_lists_.at(vertex));
}

}  // namespace graph