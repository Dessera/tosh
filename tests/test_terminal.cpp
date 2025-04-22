#include "tosh/terminal/document.hpp"
#include "tosh/terminal/event/parser.hpp"
#include <cstdio>
#include <iostream>
#include <termios.h>

int
main()
{
  using tosh::terminal::Document;
  using tosh::terminal::EventGetString;
  using tosh::terminal::EventMoveCursor;

  Document doc{ stdout, stdin, "$ " };

  doc.enter();

  while (true) {
    auto op = doc.get_op().value();

    if (auto* s = std::get_if<EventGetString>(&op); s != nullptr) {
      for (auto c : s->str) {
        doc.insert(c);
      }
    } else if (auto* c = std::get_if<EventMoveCursor>(&op); c != nullptr) {
      switch (c->direction) {
        case EventMoveCursor::Direction::LEFT:
          doc.backward();
          break;
        default:
          break;
      }
    }
  }

  doc.leave();
}