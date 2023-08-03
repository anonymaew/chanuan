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
  delete cb;
  cb = nullptr;
}

Block::Block(std::string command) : Block(command, 1){
}

Block::Block(std::string command, int interval) {
  cb = new CommandBlock(command, interval);
  output = std::vector<std::string>();
  size = {0, 0};
}

Block::Block(std::vector<Block*> children) : Block(){
  this->children = children;
}

Block::Block(const Block &other) {
  cb = other.cb;
  children = other.children;
  output = other.output;
  size = other.size;
}

Block::~Block() {
  if (cb != nullptr)
    delete cb;
}

void Block::assign_trigger(std::condition_variable *trigger) {
  if (cb != nullptr)
    cb->assign_trigger(trigger);
  else
    for (Block *child : children)
      child->assign_trigger(trigger);
}

void Block::start_recursive() {
  if (cb != nullptr)
    cb->start();
  else
    for (Block *child : children)
      child->start_recursive();
}

void Block::update() {
  if (cb != nullptr) {
    output = cb->get();
    size = cb->get_size();
  } else {
    output.clear();
    size = {0, 0};
    for (Block *child : children) {
      child->update();
      std::vector<std::string> child_output = child->get();
      std::array<int, 2> child_size = child->get_size();
      // vertical
      for (std::string col : child_output)
        output.push_back(col);
      size = {std::max(size[0], child_size[0]), size[1] + child_size[1]};
    }
  }
}

void Block::start() {
  trigger = new std::condition_variable();
  assign_trigger(trigger);
  start_recursive();
  thread = std::thread([&]() {
    while (true) {
      std::unique_lock<std::mutex> lock(mutex);
      trigger->wait(lock);
      update();
      std::cout << "\033[2J\033[1;1H" << to_string();
    }
  });
  thread.detach();
}

void Block::stop() {
  delete trigger;
  if (cb != nullptr)
    cb->stop();
  else
    for (Block *child : children)
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
