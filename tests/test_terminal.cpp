#include "tosh/terminal/document.hpp"
#include <cstdio>
#include <iostream>
#include <termios.h>

int
main()
{
  using tosh::terminal::Document;

  Document doc{ stdout, stdin, "$ " };

  doc.enter();

  while (true) {
    auto s = doc.gets();

    for (auto c : s.value()) {
      doc.insert(c);
    }
  }

  doc.leave();
}