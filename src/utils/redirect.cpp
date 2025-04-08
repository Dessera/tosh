
#include "tosh/utils/redirect.hpp"
namespace tosh::utils {

RedirectOp::RedirectOp(int src, std::string dst, RedirectOpType type)
  : _src(src)
  , _dst(std::move(dst))
  , _type(type)
{
}

}