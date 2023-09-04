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
  bool fresh;
  std::thread thread;
  std::condition_variable *trigger_consumer;
  std::chrono::seconds interval;
  enum class Border { NONE, SINGLE, ROUND } border;

  virtual void produce();

  public:
  CommandBlock();
  CommandBlock(std::string command);
  CommandBlock(std::string command, int interval);

  virtual void assign_trigger(std::condition_variable *trigger);
  virtual void start();
  virtual void stop();
  virtual std::vector<std::string> get();
  virtual std::array<int, 2> get_size();
  virtual std::string to_string();
  virtual bool is_fresh() const;
};

int str_grapheme_length(std::string str);
