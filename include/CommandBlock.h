#include <array>
#include <condition_variable>
#include <vector>
#include <thread>

#pragma once

class CommandBlock {
  protected:
  std::string command;
  std::vector<std::string> output;
  std::array<int, 2> size;
  std::thread thread;
  std::mutex mutex;
  std::condition_variable *trigger_consumer;
  std::chrono::seconds interval;

  virtual void produce();

  public:
  CommandBlock();
  CommandBlock(std::string command);
  CommandBlock(std::string command, int interval);

  virtual void assign_trigger(std::condition_variable *trigger);
  virtual void start();
  virtual void stop();
  virtual std::vector<std::string> get() const;
  virtual std::array<int, 2> get_size() const;
  virtual std::string to_string() const;
};
