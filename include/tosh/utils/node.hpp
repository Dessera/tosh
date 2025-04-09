#pragma once

#include <list>
#include <memory>
#include <ranges>
#include <vector>

namespace tosh::utils {

template<typename NodeT>
class INode
{
public:
  using NodePtr = std::shared_ptr<NodeT>;

private:
  std::list<NodePtr> _nodes{};
  NodePtr _curr{ nullptr };

public:
  INode() = default;
  virtual ~INode() = default;

  constexpr void add(const NodePtr& node) { _nodes.push_back(node); }
  constexpr void pop_back() { _nodes.pop_back(); }

  [[nodiscard]] constexpr auto& nodes() const { return _nodes; }
  [[nodiscard]] constexpr auto& nodes() { return _nodes; }

  [[nodiscard]] bool empty() const { return _nodes.empty(); }

  constexpr void current(const NodePtr& node) { _curr = node; }
  [[nodiscard]] constexpr auto& current() const { return _curr; }
  [[nodiscard]] constexpr auto& current() { return _curr; }

  std::vector<NodePtr> find_all(auto&& f) const
  {
    namespace views = std::ranges::views;
    namespace ranges = std::ranges;

    auto subs =
      nodes() |
      views::transform([&f](const auto& node) { return node->find_all(f); }) |
      views::join | ranges::to<std::vector<NodePtr>>();

    auto curr = nodes() |
                views::filter([&f](const auto& node) { return f(*node); }) |
                ranges::to<std::vector<NodePtr>>();

    curr.insert(curr.end(), subs.begin(), subs.end());
    return curr;
  }

  void remove_all(auto&& f)
  {
    nodes().remove_if([&f](const auto& node) { return f(*node); });

    for (auto& node : nodes()) {
      node->remove_all(f);
    }
  }
};

}
