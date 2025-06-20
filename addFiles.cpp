#include <iostream>
#include <filesystem>
#include <fstream>
#include "helpers.hpp"

using namespace std;
namespace fs = std::filesystem;

/**
 * Adds a file to the mini-git repository.
 * If the file is modified, it saves the object and updates the index.
 * If the file is not modified, it informs the user.
 *
 * @param path_to_file The path to the file to be added.
 */
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