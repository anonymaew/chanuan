#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

#include "CommandBlock.h"

CommandBlock::CommandBlock() {
  CommandBlock(nullptr, 1);
}

CommandBlock::CommandBlock(std::string command) {
  CommandBlock(command, 1);
}

CommandBlock::CommandBlock(std::string command, int interval) {
  this->command = command;
  this->interval = std::chrono::seconds(interval);
  output = std::vector<std::string>();
  size = {0, 0};
}

void CommandBlock::assign_trigger(std::condition_variable *trigger) {
  this->trigger = trigger;
}

std::string string_trim_end(std::string str) {
  int i = str.length() - 1;
  while (i >= 0 && (str[i] == ' ' || str[i] == '\n'))
    i--;
  return str.substr(0, i + 1);
}

int ansi_txt_len(const std::string str) {
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
  if (fp == NULL)
    throw std::runtime_error("Failed to execute command");

  char buffer[4096];
  for (int i = 0; fgets(buffer, sizeof(buffer), fp) != NULL; i++) {
    std::string buffer_str(buffer);
    buffer_str = string_trim_end(buffer_str);
    mutex.lock();
    if (i == 0) {
      output.clear();
      size = {0, 0};
    }
    output.push_back(buffer_str);
    size = {std::max(size[0], ansi_txt_len(buffer_str)), size[1] + 1};
    mutex.unlock();
  }
  pclose(fp);
}

void CommandBlock::start() {
  thread = std::thread([&]() {
    while (true) {
      auto now = std::chrono::system_clock::now();
      execute();
      trigger->notify_one();
      std::this_thread::sleep_until(now + interval);
    }
  });
  thread.detach();
}

std::vector<std::string> CommandBlock::get() const{
  return output;
}

std::array<int, 2> CommandBlock::get_size() const{
  return size;
}

std::string CommandBlock::to_string() const{
  std::string result = "";
  for (int i = 0; i < output.size(); i++)
    result += output[i] + "\n";
  return result;
}
