#pragma once

#include <any>
#include <cstddef>
#include <format>
#include <iterator>
#include <list>
#include <numeric>
#include <queue>
#include <ranges>
#include <type_traits>

namespace tosh::utils {

template<typename ItemT>
class LinkedTree
{
public:
  struct Node
  {
  private:
    ItemT _data;

    std::list<Node> _nodes;
    Node* _parent;

  public:
    Node(const ItemT& data, Node* parent = nullptr)
      : _data(data)
      , _parent(parent)
    {
    }

    [[nodiscard]] constexpr const auto& data() const { return _data; }
    [[nodiscard]] constexpr auto& data() { return _data; }
    [[nodiscard]] constexpr const auto& nodes() const { return _nodes; }
    [[nodiscard]] constexpr auto& nodes() { return _nodes; }
    [[nodiscard]] constexpr const auto& parent() const { return _parent; }
    [[nodiscard]] constexpr auto& parent() { return _parent; }
  };

  template<typename T>
  class GenericIterator
  {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::remove_cvref<T>;

  private:
    std::queue<T*> _queue{};

  public:
    GenericIterator() = default;
    GenericIterator(T* root) { _queue.push(root); }

    GenericIterator& operator++()
    {
      // end iterator
      if (_queue.empty()) {
        return *this;
      }

      auto node = _queue.front();
      _queue.pop();

      for (auto& child : node->nodes()) {
        _queue.push(&child);
      }

      return *this;
    }

    GenericIterator operator++(int)
    {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    constexpr auto& operator*() const { return node()->data(); }

    constexpr bool operator==(const GenericIterator& other) const
    {
      return node() == other.node();
    }

    [[nodiscard]] constexpr auto node() const
    {
      return _queue.empty() ? nullptr : _queue.front();
    }
  };

  using Iterator = GenericIterator<Node>;
  using ConstIterator = GenericIterator<const Node>;

private:
  Node _root;

public:
  LinkedTree(const ItemT& root)
    : _root{ root }
  {
  }

  constexpr auto begin() { return Iterator(&_root); }
  constexpr auto end() { return Iterator(); }

  [[nodiscard]] constexpr auto cbegin() const { return ConstIterator(&_root); }
  [[nodiscard]] constexpr auto cend() const { return ConstIterator(); }

  constexpr void insert(const Iterator& parent, const ItemT& item)
  {
    parent.node()->nodes().emplace_back(item, parent.node());
  }

  template<typename... Args>
  constexpr void emplace(const Iterator& parent, Args&&... args)
  {
    parent.node()->nodes().emplace_back(ItemT{ std::forward<Args>(args)... },
                                        parent.node());
  }

  void rmnode(const Iterator& it)
  {
    auto* node = it.node();
    if (node == nullptr) {
      return;
    }

    if (auto* parent = node->parent(); parent != nullptr) {
      parent->nodes().remove_if([node](auto& n) { return &n == node; });
    }
  }

  size_t filter_inplace(const Iterator& parent, auto&& pred)
  {
    auto* node = parent.node();
    if (node == nullptr) {
      return 0;
    }

    size_t sz = node->nodes().remove_if(
      [&pred](auto& node) { return pred(node.data()); });

    return std::accumulate(node->nodes().begin(),
                           node->nodes().end(),
                           sz,
                           [&pred, this](auto sz, auto& node) {
                             return sz + filter_inplace(
                                           Iterator{ &node },
                                           std::forward<decltype(pred)>(pred));
                           });
  }
};

static_assert(std::forward_iterator<LinkedTree<std::any>::Iterator>);
static_assert(std::forward_iterator<LinkedTree<std::any>::ConstIterator>);

}

template<typename ItemT, typename CharT>
  requires std::formattable<ItemT, CharT>
struct std::formatter<tosh::utils::LinkedTree<ItemT>, CharT>
{
  template<typename IT>
  using LinkedTree = tosh::utils::LinkedTree<IT>;

  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  constexpr auto format(const LinkedTree<ItemT>& tree,
                        std::format_context& ctx) const
  {
    std::string res = format_impl(tree.cbegin().node(), 0);
    return std::format_to(ctx.out(), "{}", res);
  }

private:
  constexpr std::string format_impl(const LinkedTree<ItemT>::Node* node,
                                    std::size_t depth) const
  {
    namespace views = std::ranges::views;
    namespace ranges = std::ranges;

    if (node == nullptr) {
      return std::string{};
    }

    auto subs =
      node->nodes() |
      views::transform([&](auto& n) { return format_impl(&n, depth + 2); }) |
      views::join_with('\n') | ranges::to<std::string>();

    if (subs.empty()) {
      return std::format("{}{}", std::string(depth, ' '), node->data());
    }

    return std::format("{}{}\n{}", std::string(depth, ' '), node->data(), subs);
  }
};