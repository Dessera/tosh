#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace tosh::parser {

enum class TokenType : uint8_t
{
  ROOT,      // Token Tree Root
  NORMAL,    // Normal Text
  BACKSLASH, // Backslash Escape
  QUOTE,     // Any Quote
  EXPR       // Normal Expression (Top level elements)
};

enum class TokenState : uint8_t
{
  CONTINUE,
  END,
  END_PASS,
  ERROR
};

class BaseToken
{
public:
  constexpr static std::array reserved_chars{ ' ', '\\', '\'', '"' };

protected:
  // NOLINTNEXTLINE
  TokenType _type;

public:
  BaseToken(TokenType type);
  virtual ~BaseToken() = default;

  virtual TokenState parse_next(char c) = 0;
  virtual TokenState parse_end();
  [[nodiscard]] virtual std::string to_string() const = 0;
};

class TreeToken : public BaseToken
{
protected:
  // NOLINTNEXTLINE
  std::vector<std::shared_ptr<BaseToken>> _children;
  // NOLINTNEXTLINE
  std::shared_ptr<BaseToken> _current_token{ nullptr };

public:
  TreeToken(TokenType type);
  ~TreeToken() override = default;

  TokenState parse_next(char c) override;
  TokenState parse_end() override;
  [[nodiscard]] std::string to_string() const override;

  [[nodiscard]] std::span<const std::shared_ptr<BaseToken>> tokens() const;

  void clear();
  [[nodiscard]] bool is_empty() const;

protected:
  virtual TokenState create_new_token(char c) = 0;
  void submit_current_token();
};

}
