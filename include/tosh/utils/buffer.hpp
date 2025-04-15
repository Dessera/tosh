#pragma once

#include "tosh/common.hpp"

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

namespace tosh::utils {

enum class TOSH_EXPORT CommandType : uint8_t
{
  END = 0x04,
  TAB = 0x09,
  NEXT_LINE = 0x0a,
  ESCAPE = 0x1b,
  LWIN = 0x5b,
  BACKSPACE = 0x7f,
};

/**
 * @brief Check if a character is a command
 *
 * @param c The character to check
 * @param type The type of command to check
 * @return true if the character is a command
 * @return false if the character is not a command
 */
TOSH_EXPORT bool
is_command(char c, CommandType type);

/**
 * @brief Check if a character is a regular ascii character (from ' ' to '~')
 *
 * @param c The character to check
 * @return true if the character is a regular ascii character
 * @return false if the character is not a regular ascii character
 */
TOSH_EXPORT bool
is_ascii(char c);

class TOSH_EXPORT CommandBuffer
{
private:
  std::string _buffer;
  size_t _cursor{ 0 };

  std::ostream* _out;

public:
  CommandBuffer(std::ostream& out);
  ~CommandBuffer();

  /**
   * @brief Insert a character at the cursor position
   *
   * @param c The character to insert
   */
  void insert(char c);

  /**
   * @brief Insert a string at the cursor position
   *
   * @param str The string to insert
   */
  void insert(std::string_view str);

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

  /**
   * @brief Get the character from stdin
   *
   * @return char The character read
   */
  [[nodiscard]] char getchar();

  /**
   * @brief Get the buffer content
   *
   * @return constexpr std::string The buffer content
   */
  [[nodiscard]] constexpr auto& string() const { return _buffer; }

  /**
   * @brief Get the buffer content
   *
   * @return constexpr auto& The buffer content
   */
  [[nodiscard]] constexpr auto& string() { return _buffer; }

  /**
   * @brief Get the buffer size
   *
   * @return constexpr auto The buffer size
   */
  [[nodiscard]] constexpr auto size() const { return _buffer.size(); }

  /**
   * @brief Get the buffer content from a given position
   *
   * @param start The position to start from
   * @return constexpr std::string_view The buffer content from the given
   * position
   */
  [[nodiscard]] constexpr std::string_view string_from(size_t start) const
  {
    // NOLINTNEXTLINE
    return { _buffer.c_str() + start, _buffer.size() - start };
  }

  /**
   * @brief Get the cursor position
   *
   * @return constexpr auto The cursor position
   */
  [[nodiscard]] constexpr auto cursor() const { return _cursor; }

  /**
   * @brief Check if the cursor is at the end of the buffer
   *
   * @return true if the cursor is at the end of the buffer
   * @return false if the cursor is not at the end of the buffer
   */
  [[nodiscard]] constexpr bool end() const { return _cursor == _buffer.size(); }

private:
  /**
   * @brief Enable stdin raw mode
   */
  void enable_raw_mode();

  /**
   * @brief Disable stdin raw mode
   */
  void disable_raw_mode();
};

}
