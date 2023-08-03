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
    
    Block *b1 = new Block("date",1);
    Block *b2 = new Block("date",2);
    Block *b3 = new Block("date",3);

    std::vector<Block*> blocks;
    blocks.push_back(b1);
    blocks.push_back(b2);
    blocks.push_back(b3);

    Block b(blocks);

    b.start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    b.stop();

    return 0;
}
