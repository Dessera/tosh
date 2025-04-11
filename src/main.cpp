#include <csignal>
#include <print>
#include <tosh/repl.hpp>
#include <tosh/utils/singleton.hpp>

extern "C" void
sigint_handler(int /*sig*/)
{
  using tosh::repl::Repl;
  using tosh::utils::Singleton;

  Singleton<Repl>::instance().sigint_handler();
}

int
main()
{
  using tosh::repl::Repl;
  using tosh::utils::Singleton;

  std::signal(SIGINT, sigint_handler);

  Singleton<Repl>::instance().run();
}
