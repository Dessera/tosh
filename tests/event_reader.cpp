#include "tosh/terminal/event/parser.hpp"
#include "tosh/terminal/event/reader.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <format>
#include <memory>
#include <print>
#include <termios.h>
#include <variant>

namespace {

struct PcloseCleanup
{
  // NOLINTNEXTLINE
  void operator()(std::FILE* file) { pclose(file); }
};

using FilePtr = std::unique_ptr<std::FILE, PcloseCleanup>;

FilePtr
create_test_port(std::string_view value)
{
  return FilePtr{ popen(std::format("echo -n '{}'", value).c_str(), "r") };
}

}

TEST_CASE("Test EventGetString", "[terminal][event]")
{
  using namespace std::chrono_literals;
  using namespace tosh::terminal;

  auto port = create_test_port("something");
  REQUIRE(port != nullptr);

  auto reader = EventReader::create(port.get());
  REQUIRE(reader.has_value());

  auto res = reader->read(EventFilter<EventGetString>(), 2s);
  REQUIRE(res.has_value());

  auto* eptr = std::get_if<EventGetString>(&res.value());
  REQUIRE(eptr != nullptr);
  REQUIRE(eptr->str == "something");
}

TEST_CASE("Test EventGetCursor", "[terminal][event]")
{
  using namespace std::chrono_literals;
  using namespace tosh::terminal;

  auto port = create_test_port("\x1b[1;1R");
  REQUIRE(port != nullptr);

  auto reader = EventReader::create(port.get());
  REQUIRE(reader.has_value());

  auto res = reader->read(EventFilter<EventGetCursor>(), 2s);
  REQUIRE(res.has_value());

  auto* eptr = std::get_if<EventGetCursor>(&res.value());
  REQUIRE(eptr != nullptr);
  REQUIRE(eptr->x == 0);
  REQUIRE(eptr->y == 0);
}

TEST_CASE("Test EventMoveCursor", "[terminal][event]")
{
  using namespace std::chrono_literals;
  using namespace tosh::terminal;

  auto port = create_test_port("\x1b[A");
  REQUIRE(port != nullptr);

  auto reader = EventReader::create(port.get());
  REQUIRE(reader.has_value());

  auto res = reader->read(EventFilter<EventMoveCursor>(), 2s);
  REQUIRE(res.has_value());

  auto* eptr = std::get_if<EventMoveCursor>(&res.value());
  REQUIRE(eptr != nullptr);
  REQUIRE(eptr->direction == EventMoveCursor::Direction::UP);
}

TEST_CASE("Test EventSpecialKey", "[terminal][event]")
{
  using namespace std::chrono_literals;
  using namespace tosh::terminal;

  auto port = create_test_port("\x04");
  REQUIRE(port != nullptr);

  auto reader = EventReader::create(port.get());
  REQUIRE(reader.has_value());

  auto res = reader->read(EventFilter<EventSpecialKey>(), 2s);
  REQUIRE(res.has_value());

  auto* eptr = std::get_if<EventSpecialKey>(&res.value());
  REQUIRE(eptr != nullptr);
  REQUIRE(eptr->key == EventSpecialKey::Key::KEOF);
}
