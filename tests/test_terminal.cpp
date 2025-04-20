#include "tosh/terminal/ansi.hpp"
#include "tosh/terminal/document.hpp"
#include "tosh/terminal/event.hpp"
#include <cstdio>
#include <iostream>
#include <termios.h>

int
main()
{
  using tosh::terminal::ANSIPort;
  using tosh::terminal::Document;
  using tosh::terminal::InputEventPool;

  termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &t);

  InputEventPool pool{ stdin };

  while (true) {
    char c = pool.read_input_event();

    // get cursor
    std::cout << "\x1b[6n";
    std::cout.flush();

    // get cursor
    auto c2 = pool.read_ansi_event();
    std::cerr << c2.substr(1) << std::endl;

    std::cout << c;
    std::cout.flush();
  }

  // Document doc{ stdout, stdin, "$ " };
  // auto& term = doc.terminal();

  // auto _ = doc.enter();

  // char c = term.getchar();
  // while (true) {
  //   if ((c >= ' ' && c <= '~') || c == '\n') {
  //     doc.insert(c);
  //   }

  //   if (c == '\x1b') {
  //     // ESC
  //     c = term.getchar();
  //     if (c == '[') {
  //       c = term.getchar();
  //       if (c == 'D') {
  //         doc.backward();
  //       } else if (c == 'C') {
  //         doc.forward();
  //       } else {
  //         continue;
  //       }
  //     } else {
  //       continue;
  //     }
  //   }

  //   c = term.getchar();
  // }

  // auto _ = doc.leave();
}