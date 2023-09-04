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
  fresh = false;
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

int str_display_length(std::string str) {
  int len = 0;
  bool ansi = false;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == '\033') {
      ansi = true;
      continue;
    }
    if (ansi) {
      if (str[i] == 'm')
        ansi = false;
      continue;
    }
    if ((str[i] & 0xc0) != 0x80)
      len++;
  }
  return len;
}

void process_output(std::vector<std::string> &output, std::array<int, 2> &size) {
  for (std::string &line : output)
    line = string_trim_end(line);
  std::vector<int> size_line = {};
  for (std::string line : output)
    size_line.push_back(str_display_length(line));

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
  fresh = true;
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

std::vector<std::string> CommandBlock::get() {
  fresh = false;
  return output;
}

std::array<int, 2> CommandBlock::get_size() {
  fresh = false;
  return size;
}

std::string CommandBlock::to_string() {
  fresh = false;
  std::string result = "";
  for (size_t i = 0; i < output.size(); i++)
    result += output[i] + "\n";
  return result;
}

bool CommandBlock::is_fresh() const {
  return fresh;
}
