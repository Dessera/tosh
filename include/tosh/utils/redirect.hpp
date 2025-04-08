#pragma once

#include <cstdint>
#include <string>

namespace tosh::utils {

enum class RedirectOpType : uint8_t
{
  OUT,       // >
  APPEND,    // >>
  IN,        // <
  OUT_MERGE, // >&
  IN_MERGE,  // <&
  HEREDOC,   // << (not implemented)

  UNKNOWN
};

class RedirectOp
{
private:
  int _src;
  std::string _dst;
  RedirectOpType _type;

public:
  RedirectOp(int src, std::string dst, RedirectOpType type);
};

}