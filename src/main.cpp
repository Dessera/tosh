#include "tosh/error.hpp"
#include <csignal>
#include <cstdio>
#include <print>
#include <tosh/repl.hpp>
#include <tosh/utils/singleton.hpp>

extern "C" void
signal_handler(int sig)
{
  using tosh::repl::Repl;
  using tosh::utils::Singleton;

  auto repl = Singleton<Repl>::instance();
  if (repl.has_value()) {
    repl.value()->signal_handler(sig);
  }
}

int
main()
{
  using tosh::repl::Repl;
  using tosh::utils::Singleton;

  std::signal(SIGINT, signal_handler);
  std::signal(SIGWINCH, signal_handler);

  auto repl = Singleton<Repl>::instance();
  if (!repl.has_value()) {
    repl.error().log();
    return 1;
  }

  repl.value()->run();
}
