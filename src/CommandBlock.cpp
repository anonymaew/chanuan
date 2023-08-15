#include <map>
#include <curses.h>
#include "CommandBlock.h"
#include <iostream>

CommandBlock::CommandBlock() : CommandBlock("") {
}

CommandBlock::CommandBlock(std::string command) : CommandBlock(command, 1) {
}

CommandBlock::CommandBlock(std::string command, int interval) {
  this->command = command;
  this->interval = std::chrono::seconds(interval);
  output = std::vector<std::string>();
  size = {0, 0};
  border = Border::ROUND;
}

void CommandBlock::assign_trigger(std::condition_variable *trigger) {
  trigger_consumer = trigger;
}

std::string string_trim_end(std::string str) {
  int i = str.length() - 1;
  while (i >= 0 && (str[i] == ' ' || str[i] == '\n'))
    i--;
  return str.substr(0, i + 1);
}

int str_grapheme_length(std::string str) {
  int length = 0;
  for (int i = 0; i < str.length(); i++) {
    if ((str[i] & 0xc0) != 0x80)
      length++;
  }
  return length;
}

void process_output(std::vector<std::string> &output, std::array<int, 2> &size) {
  for (std::string &line : output)
    line = string_trim_end(line);
  std::vector<int> size_line = {};
  for (std::string line : output)
    size_line.push_back(str_grapheme_length(line));

  int max_size = std::max_element(size_line.begin(), size_line.end())[0];
  for (int i = 0; i < output.size(); i++) {
    int diff = max_size - size_line[i];
    if (diff > 0)
      output[i] += std::string(diff, ' ');
  }
  size = {max_size, (int)output.size()};
}

void CommandBlock::produce() {
  if (command == "")
    return;
  FILE *fp = popen(command.c_str(), "r");
  if (fp == NULL)
    throw std::runtime_error("Failed to execute command");

  char buffer[4096];
  output.clear();
  while (fgets(buffer, sizeof(buffer), fp) != NULL)
    output.push_back(buffer);
  pclose(fp);
  process_output(output, size);
  produce_border();
}

void CommandBlock::produce_border() {
  if (border == Border::NONE)
    return;
  std::map<Border, std::vector<std::string>> border_map = {
    {Border::SINGLE, { u8"─",u8"┐",u8"│",u8"┘",u8"─",u8"└",u8"│",u8"┌"}},
    {Border::ROUND, { u8"─",u8"╮",u8"│",u8"╯",u8"─",u8"╰",u8"│",u8"╭"}}
  };
  for (size_t i = 0; i < output.size(); i++)
    output[i] =
      border_map[border][2] +
      output[i] +
      border_map[border][6];
  std::string top_border = border_map[border][0];
  std::string bottom_border = border_map[border][4];
  std::string top = border_map[border][7];
  std::string bottom = border_map[border][5];
  for (int i = 0; i < size[0]; i++) {
    top += top_border;
    bottom += bottom_border;
  }
  top += border_map[border][1];
  bottom += border_map[border][3];
  output.insert(output.begin(), top);
  output.push_back(bottom);
  size = {size[0] + 2, size[1] + 2};
}

void CommandBlock::start() {
  thread = std::thread([&]() {
    while (true) {
      auto now = std::chrono::system_clock::now();
      produce();
      trigger_consumer->notify_all();
      std::this_thread::sleep_until(now + interval);
    }
  });
  thread.detach();
}

void CommandBlock::stop() {
  thread.~thread();
}

std::vector<std::string> CommandBlock::get() const {
  return output;
}

std::array<int, 2> CommandBlock::get_size() const {
  return size;
}

std::string CommandBlock::to_string() const {
  std::string result = "";
  for (size_t i = 0; i < output.size(); i++)
    result += output[i] + "\n";
  return result;
}
