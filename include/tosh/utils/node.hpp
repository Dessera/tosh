#pragma once

#include <concepts>
#include <memory>
#include <ranges>
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

  template<std::invocable<NodePtr&> FunT>
  std::vector<NodePtr> find_all(FunT f) const
  {
    namespace views = std::ranges::views;
    namespace ranges = std::ranges;

    auto subs =
      nodes() |
      views::transform([&f](const auto& node) { return node.find_all(f); }) |
      views::join;

    auto curr = nodes() |
                views::filter([&f](const auto& node) { return f(node); }) |
                ranges::to<std::vector<NodePtr>>();

    curr.insert(curr.end(), subs.begin(), subs.end());
    return curr;
  }
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
