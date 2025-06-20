#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;
/**
 * Initializes a new mini-git repository in the current directory.
 * Creates the necessary directories and files for the repository structure.
 * Prompts the user for author name and email, which are saved in the config file.
 */
void initialize_repo(void)
{
    fs::path git_dir = ".mini-git";

    if (fs::exists(git_dir))
    {
        cout << "Repository exists already\n";
        return;
    }
    try
    {
        fs::create_directory(git_dir);
        fs::create_directories(git_dir / "objects");
        fs::create_directories(git_dir / "refs" / "heads");

        ofstream head = ofstream(git_dir / "HEAD");
        ofstream index = ofstream(git_dir / "index");
        head << "ref: refs/heads/master\n";
        head.close();
        index.close();
        cout << "Initialized empty mini-git repository in " << fs::absolute(git_dir) << "\n";
        string name, email;
        cout << "Enter author name: ";
        cin >> name;
        cout << "Enter email: ";
        cin >> email;
        ofstream author = ofstream(git_dir / "config");
        author << "[author]\n"
               << "name = " << name << "\n"
               << "email = " << email;
    }
    catch (const fs::filesystem_error &e)
    {
        cerr << "Error initializing repository: " << e.what() << '\n';
    }
}