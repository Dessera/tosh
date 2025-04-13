#pragma once

#include <cstddef>
#include <ostream>
#include <string>
namespace tosh::utils {

class CommandBuffer
{
private:
  std::string _buffer;
  std::ostream* _out;
  size_t _cursor{ 0 };

public:
  CommandBuffer(std::ostream& out);

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
  void remove(size_t n);

  /**
   * @brief Move the cursor n positions backward
   *
   * @param n The number of positions to move backward
   * @return size_t Real number of positions moved
   */
  size_t backward(size_t n);

  /**
   * @brief Move the cursor n positions forward
   *
   * @param n The number of positions to move forward
   * @return size_t Real number of positions moved
   */
  size_t forward(size_t n);

  /**
   * @brief Get the buffer content
   *
   * @return constexpr std::string The buffer content
   */
  [[nodiscard]] constexpr auto& get() const { return _buffer; }
};

}