#include <algorithm>
#include <array>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

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
  for (CommandBlock *child : children)
    child->assign_trigger(trigger);
}

void Block::start(){
  for (CommandBlock *child : children)
    child->start();
}

void Block::update(std::vector<std::string> output, std::array<int, 2> size) {
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

void Block::start_main() {
  trigger = new std::condition_variable();
  assign_trigger(trigger);
  start();
  thread = std::thread([&]() {
    while (true) {
      std::unique_lock<std::mutex> lock(mutex);
      trigger->wait(lock);
      update(output, size);
      std::cout << "\033[2J\033[1;1H" << to_string();
    }
  });
  thread.detach();
}

void Block::stop() {
  delete trigger;
  for (CommandBlock *child : children)
    child->stop();
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
