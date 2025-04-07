#pragma once

#include <memory>
#include <span>
#include <vector>

namespace tosh::utils {

template<typename NodeT>
class INode
{
public:
  using NodePtr = std::shared_ptr<NodeT>;

private:
  std::vector<NodePtr> _nodes{};

public:
  INode() = default;
  virtual ~INode() = default;

  constexpr void add(NodePtr node) { _nodes.push_back(node); }
  constexpr void pop_back() { _nodes.pop_back(); }

  [[nodiscard]] constexpr auto nodes() const -> std::span<const NodePtr>
  {
    return _nodes;
  }

  [[nodiscard]] bool empty() const { return _nodes.empty(); }
};

template<typename NodeT>
class ICursor
{
public:
  using NodePtr = std::shared_ptr<NodeT>;

private:
  NodePtr _cursor{ nullptr };

public:
  ICursor() = default;
  virtual ~ICursor() = default;

  constexpr void current(NodePtr node) { _cursor = node; }

  [[nodiscard]] constexpr auto current() const -> NodePtr { return _cursor; }
};

}
