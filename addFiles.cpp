#include <iostream>
#include <filesystem>
#include <fstream>
#include "../helpers.hpp"

using namespace std;
namespace fs = std::filesystem;

void add_file(string path_to_file)
{
    fs::path git_dir = ".mini-git";

    if (!fs::exists(git_dir))
    {
        cout << "No repository found in this directory. Use .mini-git init to initialize one.\n";
        return;
    }
    if (!fs::exists(path_to_file))
    {
        cout << "Error! File not found.";
        return;
    }
    bool wasModified = check_mod(path_to_file);
    if (!wasModified)
    {
        cout << "File is not modified. No need to add it again.\n";
        return;
    }
    string hash = save_object(path_to_file);

    ofstream indexFile(".mini-git/index", ios::app);
    if (!indexFile)
    {
        cerr << "Error: Could not update index.\n";
        return;
    }

    indexFile << path_to_file << " " << hash << "\n";
}