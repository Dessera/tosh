#include "tosh/terminal/ansi.hpp"
#include "tosh/terminal/document.hpp"
#include <cstdio>
#include <iostream>

int
main()
{
  using tosh::terminal::ANSIPort;
  using tosh::terminal::Document;

  Document doc{ stdout, stdin, "$ " };
  auto& term = doc.terminal();

  auto _ = doc.enter();

  char c = term.getchar();
  while (true) {
    if ((c >= ' ' && c <= '~') || c == '\n') {
      doc.insert(c);
    }

    if (c == '\x1b') {
      // ESC
      c = term.getchar();
      if (c == '[') {
        c = term.getchar();
        if (c == 'D') {
          doc.backward();
        } else if (c == 'C') {
          doc.forward();
        } else {
          continue;
        }
      } else {
        continue;
      }
    }

    c = term.getchar();
  }

  auto _ = doc.leave();
}