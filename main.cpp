#include <iostream>
#include <string>
#include <vector>
#include "commands.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: mini-git <command> [options]\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "init")
    {
        initialize_repo();
    }
    else if (command == "add")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: mini-git add <filename>\n";
            return 1;
        }
        add_file(argv[2]);
    }
    else if (command == "commit")
    {
        if (argc < 4 || std::string(argv[2]) != "-m")
        {
            std::cerr << "Usage: mini-git commit -m \"<message>\"\n";
            return 1;
        }
        commit(argv[3]);
    }
    else if (command == "log")
    {
        log();
    }
    else if (command == "branch")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: mini-git branch <branch-name>\n";
            return 1;
        }
        branch(argv[2]);
    }
    else if (command == "checkout")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: mini-git checkout <branch-name || commitID>\n";
            return 1;
        }
        checkout(argv[2], true);
    }
    else if (command == "merge")
    {
        if (argc < 4)
        {
            std::cerr << "Usage: mini-git checkout <branch-name || commitID>\n";
            return 1;
        }
        merge(argv[3]);
    }
    else if (command == "diff")
    {
        if (argc < 4)
        {
            std::cerr << "Usage: mini-git diff <commitID1> <commitID2>\n";
            return 1;
        }
        diffCommits(argv[2], argv[3]);
    }
    else
    {
        std::cerr << "Unknown command: " << command << "\n";
        return 1;
    }

    return 0;
}
