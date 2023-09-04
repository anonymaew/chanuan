#include <iostream>
#include <map>

#include "Block.h"
#include "CommandBlock.h"

Block::Block() : Block(""){
}

Block::Block(std::string command) : Block(command, 1){
}

Block::Block(std::string command, int interval) : CommandBlock(command, interval) {
  children = { new CommandBlock(command, interval) };
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
  produce_border();
  fresh = true;
}

void Block::produce_border() {
  if (border == Border::NONE)
    return;
  std::map<Border, std::vector<std::string>> border_map = {
    {Border::SINGLE, { u8"─",u8"┐",u8"│",u8"┘",u8"─",u8"└",u8"│",u8"┌"}},
    {Border::ROUND, { u8"─",u8"╮",u8"│",u8"╯",u8"─",u8"╰",u8"│",u8"╭"}}
  };
  for (size_t i = 0; i < output.size(); i++)
    output[i] =
      border_map[border][2] +
      output[i] +
      border_map[border][6];
  std::string top_border = border_map[border][0];
  std::string bottom_border = border_map[border][4];
  std::string top = border_map[border][7];
  std::string bottom = border_map[border][5];
  for (int i = 0; i < size[0]; i++) {
    top += top_border;
    bottom += bottom_border;
  }
  top += border_map[border][1];
  bottom += border_map[border][3];
  output.insert(output.begin(), top);
  output.push_back(bottom);
  size = {size[0] + 2, size[1] + 2};
}

bool Block::is_child_fresh() const {
  for (CommandBlock *child : children)
    if (child->is_fresh())
      return true;
  return false;
}

void Block::start(){
  for (CommandBlock *child : children)
    child->start();
  thread = std::thread([&]() {
    while (true) {
      std::unique_lock<std::mutex> lock(mutex);
      trigger_producer->wait(lock, [&]() { return is_child_fresh(); });
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
      trigger_consumer->wait(lock, [&]() { return is_fresh(); });
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

std::vector<std::string> Block::get() {
  return CommandBlock::get();
}

std::array<int, 2> Block::get_size() {
  return CommandBlock::get_size();
}

std::string Block::to_string() {
  return CommandBlock::to_string();
}

bool Block::is_fresh() const {
  return CommandBlock::is_fresh();
}
