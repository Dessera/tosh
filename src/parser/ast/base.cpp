#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

IToken::IToken(TokenType type)
  : _type(type)
{
}

std::string
IToken::string() const
{
  return {};
}

parser::ParseState
IToken::on_invalid(parser::TokenParseContext& /*ctx*/, char /*c*/)
{
  return parser::ParseState::INVALID;
}

}
