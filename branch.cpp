#include <fstream>
#include <iostream>
#include <string>

using namespace std;
/**
 * branch - Create a new branch in the mini-git repository.
 * This function creates a new branch with the specified name,
 * copying the current commit hash from the current branch.
 * If the branch already exists, it outputs an error message.
 * If HEAD is not found or has an invalid format, it also outputs an error message.
 * The new branch is created in the .mini-git/refs/heads directory.
 */
void branch(const string &branchName)
{
    string newBranchPath = ".mini-git/refs/heads/" + branchName;

    // Check if the branch already exists
    ifstream existing(newBranchPath);
    if (existing)
    {
        cerr << "fatal: branch '" << branchName << "' already exists.\n";
        return;
    }

    // Read HEAD to get current branch
    ifstream headFile(".mini-git/HEAD");
    if (!headFile)
    {
        cerr << "fatal: HEAD not found.\n";
        return;
    }

    string refLine;
    getline(headFile, refLine);
    if (refLine.find("ref: ") != 0)
    {
        cerr << "fatal: invalid HEAD format.\n";
        return;
    }

    string currentBranchRef = ".mini-git/" + refLine.substr(5); // strip "ref: "

    // 3. Read current commit hash from current branch ref
    ifstream currentBranchFile(currentBranchRef);
    if (!currentBranchFile)
    {
        cerr << "fatal: current branch file not found.\n";
        return;
    }

    string currentCommitHash;
    getline(currentBranchFile, currentCommitHash);

    // 4. Create new branch file and write the commit hash
    ofstream newBranchFile(newBranchPath);
    if (!newBranchFile)
    {
        cerr << "fatal: could not create branch '" << branchName << "'.\n";
        return;
    }

    newBranchFile << currentCommitHash;
    cout << "Branch '" << branchName << "' created at " << currentCommitHash << ".\n";
}
