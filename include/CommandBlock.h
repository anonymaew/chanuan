#include <array>
#include <chrono>
#include <string>
#include <vector>

#pragma once

class CommandBlock {
  private:
  std::string command;
  std::vector<std::string> output;
  std::array<int, 2> size;
  std::chrono::seconds interval;
  std::chrono::time_point<std::chrono::system_clock> last_update;
  enum class Type { COMMAND, BLOCK } type;

  std::vector<CommandBlock> children;
  enum class Direction { HORIZONTAL, VERTICAL } direction;

  public:
  CommandBlock(std::string command);
  CommandBlock(std::string command, int interval);
  CommandBlock(std::vector<CommandBlock> commands);

  void execute();
  std::vector<std::string> get() const;
  std::array<int, 2> get_size() const;
  std::string to_string();
};
