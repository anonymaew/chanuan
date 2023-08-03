#include "CommandBlock.h"

#pragma once

class Block : protected CommandBlock {
  protected:
  std::vector<CommandBlock*> children;

  virtual void update(std::vector<std::string> output, std::array<int, 2> size);
  
  public:
  Block();
  Block(std::string command);
  Block(std::string command, int interval);
  Block(std::vector<CommandBlock*> children);
  void start_main();

  virtual void assign_trigger(std::condition_variable *trigger);
  virtual void start();
  virtual void stop();
  virtual std::vector<std::string> get() const;
  virtual std::array<int, 2> get_size() const;
  virtual std::string to_string() const;
};
