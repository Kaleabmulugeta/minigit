#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <filesystem>
#include <sstream>
#include "helpers.hpp"

using namespace std;

/**
 * diffCommits - Compare two commits and show differences in tracked files.
 * It lists files that are unchanged, modified, added, or removed between the two commits.
 */
void diffCommits(const string &commitHash1, const string &commitHash2)
{
    string commit1 = readFile(".mini-git/objects/" + commitHash1);
    string commit2 = readFile(".mini-git/objects/" + commitHash2);

    string treeHash1 = getTreeHashFromCommit(commit1);
    string treeHash2 = getTreeHashFromCommit(commit2);

    if (treeHash1.empty() || treeHash2.empty())
    {
        cerr << "Error: One or both commits missing tree.\n";
        return;
    }

    map<string, string> tree1 = parseTreeObject(readFile(".mini-git/objects/" + treeHash1));
    map<string, string> tree2 = parseTreeObject(readFile(".mini-git/objects/" + treeHash2));

    set<string> allFiles;
    for (auto &[file, _] : tree1)
        allFiles.insert(file);
    for (auto &[file, _] : tree2)
        allFiles.insert(file);

    for (const string &filename : allFiles)
    {
        int in1 = tree1.count(filename);
        int in2 = tree2.count(filename);

        if (in1 >= 1 && in2 >= 1)
        {
            if (trim(tree1[filename]) == trim(tree2[filename]))
            {
                cout << "UNCHANGED: " << filename << '\n';
            }
            else
            {
                cout << "MODIFIED:  " << filename << '\n';
            }
        }
        else if (in1 >= 1 && in2 == 0)
        {
            cout << "REMOVED:   " << filename << '\n';
        }
        else if (in1 == 0 && in2 >= 1)
        {
            cout << "ADDED:     " << filename << '\n';
        }
    }
}
