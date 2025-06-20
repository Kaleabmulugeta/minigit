#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>
#include <sstream>
#include <vector>
#include <openssl/sha.h>
#include "../helpers.hpp"

using namespace std;
namespace fs = std::filesystem;

// Utility functions

void checkout(const string &ref, bool force = false)
{
    string commitHash;
    string refPath = ".mini-git/refs/heads/" + ref;
    bool isBranch = false;

    // Resolve ref
    if (fileExists(refPath))
    {
        string hd = readFile(".mini-git/HEAD");
        if (hd == "ref: refs/heads/" + ref)
        {
            cout << "Already on branch " << ref << "\n";
            return;
        }
        commitHash = trim(readFile(refPath));
        isBranch = true;
    }
    else if (fileExists(".mini-git/objects/" + ref))
    {
        commitHash = ref;
        cout << "Note: You are now in a detached HEAD state at commit " << ref << ".\n";
    }
    else
    {
        cerr << "Error: No such branch or commit: " << ref << "\n";
        return;
    }

    // Get tree of target commit
    string commitContent = readFile(".mini-git/objects/" + commitHash);
    string treeHash = getTreeHashFromCommit(commitContent);
    if (treeHash.empty())
    {
        cerr << "Invalid commit: tree not found.\n";
        return;
    }
    string treeContent = readFile(".mini-git/objects/" + treeHash);
    map<string, string> targetTrackedFiles = parseTreeObject(treeContent); // filename -> blobHash

    map<string, string> currentTrackedFiles = getCurrentTrackedFiles();

    // Check modified tracked files
    if (!force)
    {
        vector<string> modifiedFiles = getModifiedFiles(currentTrackedFiles);
        if (!modifiedFiles.empty())
        {
            cerr << "Error: Cannot checkout due to modified tracked files:\n";
            for (const auto &file : modifiedFiles)
                cerr << "  " << file << "\n";
            cerr << "Commit, stash or use --force to proceed.\n";
            return;
        }
    }

    // Check untracked files that would be overwritten
    if (!force)
    {
        vector<string> conflicts;
        for (const auto &[filename, _] : targetTrackedFiles)
        {
            if (fileExists(filename) && currentTrackedFiles.count(filename) == 0)
            {
                conflicts.push_back(filename);
            }
        }
        if (!conflicts.empty())
        {
            cerr << "Error: The following untracked files would be overwritten by checkout:\n";
            for (const auto &file : conflicts)
                cerr << "  " << file << "\n";
            cerr << "Please remove, stash, or use --force to proceed.\n";
            return;
        }
    }

    // Remove files from current tree not in new tree
    try
    {
        for (const auto &[filename, _] : currentTrackedFiles)
        {
            if (targetTrackedFiles.count(filename) == 0)
            {
                fs::remove(filename);
                cout << "Removed: " << filename << "\n";
            }
        }

        // Write files from new tree
        for (const auto &[filename, blobHash] : targetTrackedFiles)
        {
            string blobContent = readFile(".mini-git/objects/" + blobHash);
            writeFile(filename, blobContent);
            cout << "Updated: " << filename << "\n";
        }
    }
    catch (const runtime_error &e)
    {
        cerr << "Error during checkout: " << e.what() << "\n";
        return;
    }

    // Update HEAD
    if (isBranch)
        writeFile(".mini-git/HEAD", "ref: refs/heads/" + ref);
    else
        writeFile(".mini-git/HEAD", commitHash);

    cout << "Switched to " << (isBranch ? "branch " : "commit ") << ref << " successfully.\n";
}
