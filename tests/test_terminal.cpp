#include "tosh/error.hpp"
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
  using tosh::terminal::EventSpecialKey;

  Document doc{ stdout, stdin, "$ " };

  while (true) {
    LOGERR_EXIT(doc.enter());

    while (true) {
      auto res = doc.get_op();
      if (!res.has_value()) {
        res.error().log();
        break;
      }

      auto op = res.value();

      if (auto* s = std::get_if<EventGetString>(&op); s != nullptr) {
        for (auto c : s->str) {
          if (auto res = doc.insert(c); !res.has_value()) {
            res.error().log();
            goto error;
          }
        }
      } else if (auto* c = std::get_if<EventMoveCursor>(&op); c != nullptr) {
        if (c->direction == EventMoveCursor::Direction::LEFT) {
          auto _ = doc.backward();
        } else if (c->direction == EventMoveCursor::Direction::RIGHT) {
          auto _ = doc.forward();
        }
      } else if (auto* c = std::get_if<EventSpecialKey>(&op); c != nullptr) {
        if (c->key == EventSpecialKey::Key::BACKSPACE) {
          auto _ = doc.remove();
        }
      }
    }

  error:
    doc.leave();
  }
}