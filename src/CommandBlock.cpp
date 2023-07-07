#include <err.h>
#include <iostream>
#include <string>
#include <vector>

#include "CommandBlock.h"

CommandBlock::CommandBlock(std::string command) {
  this->command = command;
  output = std::vector<std::string>();
  last_update = 0;
  width = 0;
  height = 0;
}

std::string string_trim_end(std::string str) {
  int i = str.length() - 1;
  while (i >= 0 && (str[i] == ' ' || str[i] == '\n'))
    i--;
  return str.substr(0, i + 1);
}

size_t ansi_txt_len(const std::string str) {
  size_t len = 0;
  int ansi = 0;
  for (int i = 0; i < str.length(); i++) {
    if (ansi) {
      if (str[i] == 'm')
        ansi = 0;
    }
    else {
      if (str[i] == '\033')
        ansi = 1;
      else
        len++;
    }
  }
  return len;
}

void CommandBlock::execute() {
  FILE *fp = popen(command.c_str(), "r");
  std::string result = "";
  if (fp == NULL)
    err(1, "failed executing %s", command.c_str());

  char buffer[4096];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    std::string buffer_str(buffer);
    buffer_str = string_trim_end(buffer_str);
    int len = ansi_txt_len(buffer_str);
    output.push_back(buffer_str);
    if (len > width)
      width = len;
    height++;
  }
  pclose(fp);
}

std::vector<std::string> CommandBlock::get() const{
  return output;
}

std::string CommandBlock::to_string() {
  std::string result = "";
  for (int i = 0; i < output.size(); i++)
    result += output[i] + "\n";
  return result;
}
