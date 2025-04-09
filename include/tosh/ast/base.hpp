#pragma once

#include <list>
#include <memory>
namespace tosh::ast {

class ParseContext;

enum class ParseStatus : uint8_t
{
  CONTINUE,
  END,
  END_NEXT,
  REPEAT,
  ERROR,
};

class IToken
{
public:
  using Ptr = std::shared_ptr<IToken>;
  using WeakPtr = std::weak_ptr<IToken>;

private:
  std::list<Ptr> _nodes;
  WeakPtr _parent;

public:
  virtual ~IToken() = default;

  virtual ParseStatus on_continue(ParseContext& ctx, char c);
  virtual ParseStatus on_invalid(ParseContext& ctx, char c);
};

}