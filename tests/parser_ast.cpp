#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/root.hpp"

#include <catch2/catch_test_macros.hpp>

#include <format>
#include <memory>
#include <string_view>

namespace {

using namespace tosh::ast;

bool
test_parse(tosh::ast::Root::Ptr& root,
           // NOLINTNEXTLINE
           std::string_view input,
           std::string_view expected)
{
  root->clear();

  for (auto c : input) {
    if (root->parse_next(c) == tosh::ast::ParseState::END) {
      break;
    }
  }

  root->parse_next('\0');

  return std::format("{}", *root) == expected;
}

}

TEST_CASE("Test AST Token", "[parser][ast]")
{
  Token::Ptr root = std::make_shared<Root>();

  REQUIRE(test_parse(root,
                     "ls -lah",
                     R"(ROOT:
  EXPR:
    TEXT: ls
  EXPR:
    TEXT: -lah)"));

  REQUIRE(test_parse(root, "ls ~/", R"(ROOT:
  EXPR:
    TEXT: ls
  EXPR:
    HOME: ~/)"));

  REQUIRE(test_parse(root,
                     R"(ec"ho" 'hello \' \" world' \\ \' \" \
from dessera)",
                     R"(ROOT:
  EXPR:
    TEXT: ec
    QUOTE:
      TEXT: ho
  EXPR:
    QUOTE:
      TEXT: hello 
      BACKSLASH: '
      TEXT:  
      TEXT: \" world
  EXPR:
    BACKSLASH: \
  EXPR:
    BACKSLASH: '
  EXPR:
    BACKSLASH: "
  EXPR:
    BACKSLASH: 
    TEXT: from
  EXPR:
    TEXT: dessera)"));

  REQUIRE(test_parse(
    root, "ls > out.txt 2> error.txt >> append.txt >>> invalid.txt", R"(ROOT:
  EXPR:
    TEXT: ls
  EXPR:
    REDIRECT:
      REDIRECT_OP: >
      REDIRECT_DEST: out.txt
  EXPR:
    REDIRECT:
      REDIRECT_SRC: 2
      REDIRECT_OP: >
      REDIRECT_DEST: error.txt
  EXPR:
    REDIRECT:
      REDIRECT_OP: >>
      REDIRECT_DEST: append.txt
  EXPR:
    TEXT: >>>
  EXPR:
    TEXT: invalid.txt)"));
}