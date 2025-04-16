#include "tosh/terminal/ansi.hpp"
#include "tosh/terminal/document.hpp"
#include <cstdio>

int
main()
{
  using tosh::terminal::ANSIPort;
  using tosh::terminal::Document;

  Document doc{ stdout, stdin, "$ " };

  char c = std::getchar();
  while (true) {

    if (c >= ' ' && c <= '~') {
      doc.insert(c);
    }

    if (c == '\x1b') {
      // ESC
      c = std::getchar();
      if (c == '[') {
        c = std::getchar();
        if (c == 'D') {
          doc.backward();
        } else {
          continue;
        }
      } else {
        continue;
      }
    }

    c = std::getchar();
  }
}