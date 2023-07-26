#include <array>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

#pragma once

class CommandBlock {
  private:
  std::string command;
  std::vector<std::string> output;
  std::array<int, 2> size;
  std::thread thread;
  std::mutex mutex;
  std::condition_variable *trigger;
  std::chrono::seconds interval;

  public:
  CommandBlock();
  CommandBlock(std::string command);
  CommandBlock(std::string command, int interval);
  void assign_trigger(std::condition_variable *trigger);

  void execute();
  void start();
  void stop();
  std::vector<std::string> get() const;
  std::array<int, 2> get_size() const;
  std::string to_string() const;
};
