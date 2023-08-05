#include "CommandBlock.h"

#pragma once

class Block : public CommandBlock {
  protected:
  std::vector<CommandBlock*> children;
  std::thread thread_printer;
  std::condition_variable *trigger_producer;

  void produce();
  void start();
  void stop();

  public:
  Block();
  Block(std::string command);
  Block(std::string command, int interval);
  Block(std::vector<CommandBlock*> children);
  void start_main();
  void stop_main();

  void assign_trigger(std::condition_variable *trigger);
  std::vector<std::string> get() const;
  std::array<int, 2> get_size() const;
  std::string to_string() const;
};
