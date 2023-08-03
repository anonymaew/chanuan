#include <array>
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

#include "CommandBlock.h"

#pragma once

class Block {
  private:
  CommandBlock *cb;
  std::vector<Block*> children;
  std::vector<std::string> output;
  std::array<int, 2> size;
  std::condition_variable *trigger;
  std::mutex mutex;
  std::thread thread;

  void assign_trigger(std::condition_variable *trigger);
  void start_recursive();
  void update();
  
  public:
  Block();
  Block(std::string command);
  Block(std::string command, int interval);
  Block(std::vector<Block*> children);
  Block(const Block &other);
  ~Block();

  void start();
  void stop();
  std::array<int, 2> get_size() const;
  std::vector<std::string> get() const;
  std::string to_string() const;
};
