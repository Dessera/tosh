#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
namespace tosh::utils {

enum class CommandType : uint8_t
{
  TAB = 0x09,
  NEXT_LINE = 0x0a,
  ESCAPE = 0x1b,
  LWIN = 0x5b,
  BACKSPACE = 0x7f,
};

bool
is_command(char c, CommandType type);

bool
is_ascii(char c);

class CommandBuffer
{
private:
  std::string _buffer;
  size_t _cursor{ 0 };

  std::ostream* _out;

public:
  CommandBuffer(std::ostream& out);
  ~CommandBuffer();

  /**
   * @brief Clear the buffer
   */
  void clear();

  /**
   * @brief Insert a character at the cursor position
   *
   * @param c The character to insert
   */
  void insert(char c);

  /**
   * @brief Insert a character only in stream
   *
   * @param c The character to insert
   */
  void stream_insert(char c);

  /**
   * @brief Remove n characters from the buffer starting at the cursor position
   * (backspace)
   *
   * @param n The number of characters to remove
   */
  void remove();

  /**
   * @brief Move the cursor n positions backward
   *
   * @param n The number of positions to move backward
   * @return size_t Real number of positions moved
   */
  size_t backward();

  /**
   * @brief Move the cursor n positions forward
   *
   * @param n The number of positions to move forward
   * @return size_t Real number of positions moved
   */
  size_t forward();

  [[nodiscard]] char getchar();

  /**
   * @brief Get the buffer content
   *
   * @return constexpr std::string The buffer content
   */
  [[nodiscard]] constexpr auto& string() const { return _buffer; }
  [[nodiscard]] constexpr auto& string() { return _buffer; }
  [[nodiscard]] constexpr auto size() const { return _buffer.size(); }

  [[nodiscard]] constexpr std::string_view string_from(size_t start) const
  {
    // NOLINTNEXTLINE
    return { _buffer.c_str() + start, _buffer.size() - start };
  }

  [[nodiscard]] constexpr auto cursor() const { return _cursor; }

  [[nodiscard]] constexpr bool end() const { return _cursor == _buffer.size(); }

private:
  void enable_raw_mode();
  void disable_raw_mode();
};

}