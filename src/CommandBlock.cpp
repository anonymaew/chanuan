#include <algorithm>
#include <array>
#include <chrono>
#include <err.h>
#include <iostream>
#include <string>
#include <vector>

#include "CommandBlock.h"

CommandBlock::CommandBlock(std::string command) {
  this->command = command;
  output = std::vector<std::string>();
  size = {0, 0};
  interval = std::chrono::seconds(1);
  last_update = std::chrono::system_clock::from_time_t(0);
  type = Type::COMMAND;
}

CommandBlock::CommandBlock(std::string command, int interval) {
  *this = CommandBlock(command);
  this->interval = std::chrono::seconds(interval);
}

CommandBlock::CommandBlock(std::vector<CommandBlock> commands) {
  *this = CommandBlock("");
  this->children = commands;
  type = Type::BLOCK;
  direction = Direction::VERTICAL;
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
  auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
  if (now - last_update < interval)
    return;

  last_update = now;
  output.clear();
  size = {0, 0};
  if (type == Type::COMMAND) {
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
      size = {std::max(size[0], len), size[1] + 1};
    }
    pclose(fp);
  }
  else {
    pthread_t threads[children.size()];
    for (int i = 0; i < children.size(); i++) {
      pthread_create(&threads[i], NULL, [](void *arg) -> void * {
        CommandBlock *cb = (CommandBlock *)arg;
        cb->execute();
        return NULL;
      }, (void *)&children[i]);
    }
    for (int i = 0; i < children.size(); i++)
      pthread_join(threads[i], NULL);
    for (CommandBlock cb : children) {
      if (direction == Direction::VERTICAL) {
        size = {std::max(size[0], cb.size[0]), size[1] + cb.size[1]};
        output.insert(output.end(), cb.output.begin(), cb.output.end());
      }
      else ;
    }
  }
}

std::vector<std::string> CommandBlock::get() const{
  return output;
}

std::array<int, 2> CommandBlock::get_size() const{
  return size;
}

std::string CommandBlock::to_string() {
  std::string result = "";
  for (int i = 0; i < output.size(); i++)
    result += output[i] + "\n";
  return result;
}
