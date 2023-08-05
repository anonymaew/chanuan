#include <iostream>
#include "Block.h"

int main(int argc, char *argv[])
{
    // std::ifstream scripts("scripts.txt");

    // for (std::string line; std::getline(scripts, line);)
    //     blocks.push_back(CommandBlock(line));
    
    CommandBlock *b1 = new CommandBlock("date",1);
    CommandBlock *b2 = new CommandBlock("date",2);
    CommandBlock *b3 = new CommandBlock("date",3);
    CommandBlock *b4 = new CommandBlock("date",4);
    std::vector<CommandBlock*> bl({b1, b2});
    Block *bb1 = new Block(bl);
    std::vector<CommandBlock*> bl2({bb1, b3});
    Block *bb2 = new Block(bl2);
    std::vector<CommandBlock*> bl3 = {bb2, b4};
    Block *bb3 = new Block(bl3);

    bb3->start_main();
    std::this_thread::sleep_for(std::chrono::seconds(20));
    bb3->stop_main();

    return 0;
}
