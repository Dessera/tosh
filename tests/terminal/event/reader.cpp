#include "tosh/terminal/event/reader.hpp"
#include "tosh/terminal/event/parser.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <format>
#include <memory>
#include <print>
#include <termios.h>

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

void
send_data(std::FILE* file, std::string_view str)
{
  fwrite(str.data(), 1, str.size(), file);
  fflush(file);
}

}

TEST_CASE("Test EventGetString", "[terminal][event][reader]")
{
  using namespace std::chrono_literals;
  using namespace tosh::terminal;

  // get tmp name
  auto file = create_test_port("something");
  REQUIRE(file != nullptr);

  EventReader reader{ file.get() };

  auto event = reader.read(EventFilter<EventGetString>(), 2s);
  REQUIRE(event.has_value());
}