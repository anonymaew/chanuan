#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "Block.h"

int main(int argc, char *argv[])
{
    // std::ifstream scripts("scripts.txt");

    // for (std::string line; std::getline(scripts, line);)
    //     blocks.push_back(CommandBlock(line));
    
    CommandBlock *b1 = new CommandBlock("date",1);
    CommandBlock *b2 = new CommandBlock("date",2);
    CommandBlock *b3 = new CommandBlock("date",3);

    std::vector<CommandBlock*> blocks;
    blocks.push_back(b1);
    blocks.push_back(b2);
    blocks.push_back(b3);

    Block b(blocks);

    b.start_main();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    b.stop();

    return 0;
}
