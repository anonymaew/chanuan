#include <condition_variable>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include "CommandBlock.h"

int main(int argc, char *argv[])
{
    // std::ifstream scripts("scripts.txt");

    // for (std::string line; std::getline(scripts, line);)
    //     blocks.push_back(CommandBlock(line));
    
    std::condition_variable cv;
    std::mutex cvm;

    std::vector<CommandBlock*> blocks;
    blocks.push_back(new CommandBlock("python3 -c 'import time; time.sleep(1); print(time.time());'", 3));
    blocks.push_back(new CommandBlock("python3 -c 'import time; print(time.time());'", 3));
    blocks.push_back(new CommandBlock("python3 -c 'import time; print(time.time());'", 2));
    for (CommandBlock* cb : blocks) {
        cb->assign_trigger(&cv);
        cb->start();
    }

    while (true) {
        std::unique_lock<std::mutex> lock(cvm);
        cv.wait(lock);

        std::cout << "\033[2J\033[1;1H";
        for (CommandBlock* cb : blocks)
            std::cout << cb->to_string();
    }

    return 0;
}
