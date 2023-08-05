#include "CommandBlock.h"

CommandBlock::CommandBlock() : CommandBlock("") {
}

CommandBlock::CommandBlock(std::string command) : CommandBlock(command, 1) {
}

CommandBlock::CommandBlock(std::string command, int interval) {
  this->command = command;
  this->interval = std::chrono::seconds(interval);
  output = std::vector<std::string>();
  size = {0, 0};
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

int ansi_txt_len(const std::string str) {
  size_t len = 0;
  int ansi = 0;
  for (size_t i = 0; i < str.length(); i++) {
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

void CommandBlock::produce() {
  FILE *fp = popen(command.c_str(), "r");
  if (fp == NULL)
    throw std::runtime_error("Failed to execute command");

  char buffer[4096];
  std::vector<std::string> output_temp;
  std::array<int, 2> size_temp = {0, 0};
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    std::string buffer_str(buffer);
    buffer_str = string_trim_end(buffer_str);
    output_temp.push_back(buffer_str);
    size_temp = {std::max(size_temp[0], ansi_txt_len(buffer_str)), size_temp[1] + 1};
  }
  pclose(fp);
  output = output_temp;
  size = size_temp;
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

std::vector<std::string> CommandBlock::get() const{
  return output;
}

std::array<int, 2> CommandBlock::get_size() const{
  return size;
}

std::string CommandBlock::to_string() const{
  std::string result = "";
  for (size_t i = 0; i < output.size(); i++)
    result += output[i] + "\n";
  return result;
}
