#include <string>
#include <vector>

#pragma once

class CommandBlock {
  private:
  std::string command;
  std::vector<std::string> output;
  unsigned long long last_update;
  int width;
  int height;

  public:
  CommandBlock(std::string command);
  void execute();
  std::vector<std::string> get() const;
  std::string to_string();
};
