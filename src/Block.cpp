#include <iostream>

#include "Block.h"
#include "CommandBlock.h"

Block::Block() : Block(""){
}

Block::Block(std::string command) : Block(command, 1){
}

Block::Block(std::string command, int interval) : CommandBlock(command, interval) {
}

Block::Block(std::vector<CommandBlock*> children) : Block(){
  this->children = children;
}

void Block::assign_trigger(std::condition_variable *trigger){
  CommandBlock::assign_trigger(trigger);
  trigger_producer = new std::condition_variable();
  for (CommandBlock *child : children)
    child->assign_trigger(trigger_producer);
}

void Block::produce() {
  this->output.clear();
  this->size = {0, 0};
  for (CommandBlock *child : children) {
    std::vector<std::string> child_output = child->get();
    std::array<int, 2> child_size = child->get_size();
    // vertical
    for (std::string col : child_output) {
      this->output.push_back(col);
      this->size = {std::max(size[0], child_size[0]), this->size[1] + child_size[1]};
    }
  }
}

void Block::start(){
  for (CommandBlock *child : children)
    child->start();
  thread = std::thread([&]() {
    while (true) {
      std::unique_lock<std::mutex> lock(mutex);
      trigger_producer->wait(lock);
      produce();
      trigger_consumer->notify_all();
    }
  });
  thread.detach();
}

void Block::start_main() {
  assign_trigger(new std::condition_variable());
  start();
  thread_printer = std::thread([&]() {
    while (true) {
      std::unique_lock<std::mutex> lock(mutex);
      trigger_producer->wait(lock);
      std::cout << "\033[2J\033[1;1H" << to_string();
    }
  });
  thread_printer.detach();
}

void Block::stop(){
  CommandBlock::stop();
  delete trigger_producer;
  for (CommandBlock *child : children)
    child->stop();
}

void Block::stop_main(){
  stop();
  delete trigger_consumer;
  thread_printer.~thread();
}

std::array<int, 2> Block::get_size() const {
  return size;
}

std::vector<std::string> Block::get() const {
  return output;
}

std::string Block::to_string() const {
  std::string result = "";
  for (std::string line : output)
    result += line + "\n";
  return result;
}
