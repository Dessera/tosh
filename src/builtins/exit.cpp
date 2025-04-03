
#include "desh/builtins/exit.hpp"

#include <cstdlib>

namespace desh::builtins {

int
Exit::execute(const std::vector<std::string_view>& /*args*/)
{
  std::exit(EXIT_SUCCESS);
}

}