#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "CommandBlock.h"

int main(int argc, char *argv[])
{
    std::ifstream scripts("scripts.txt");
    std::vector<CommandBlock> blocks;

    for (std::string line; std::getline(scripts, line);)
        blocks.push_back(CommandBlock(line));

    for (CommandBlock& cb : blocks)
        cb.execute();

    for (CommandBlock cb : blocks)
        std::cout << cb.to_string() << std::endl;

    return 0;
}
