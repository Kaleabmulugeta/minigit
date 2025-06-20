#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include "helpers.hpp"

/**
 * log - Display the commit history of the repository.
 * It reads the HEAD reference, traverses the commit history,
 * and prints commit details including hash, author, date, and message.
 * If HEAD is not found or no commits exist, it displays an error message.
 * If the commit object is not found, it also displays an error message.
 * The output is formatted to show each commit in reverse chronological order.
 * Each commit is displayed with its hash, author, date, and message.
 * The function handles errors gracefully, providing feedback to the user.
 */
void log(void)
{
    // Step 1: Read HEAD
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

    string refPath = ".mini-git/" + refLine.substr(5);
    string commitHash = readFile(refPath);
    if (commitHash.empty())
    {
        cerr << "fatal: no commits yet.\n";
        return;
    }

    // Step 2: Traverse commits
    while (!commitHash.empty())
    {
        string commitPath = ".mini-git/objects/" + commitHash;
        string content = readFile(commitPath);
        if (content.empty())
        {
            cerr << "fatal: commit object not found: " << commitHash << "\n";
            break;
        }

        string author, date, message, parent;
        istringstream iss(content);
        string line;
        while (getline(iss, line))
        {
            if (line.find("author ") == 0)
            {
                author = line.substr(7);
            }
            else if (line.find("date ") == 0)
            {
                date = line.substr(5);
            }
            else if (line.find("message ") == 0)
            {
                message = line.substr(8);
            }
            else if (line.find("parent ") == 0)
            {
                parent = line.substr(7);
            }
        }

        cout << "commit " << commitHash << "\n";
        cout << "Author: " << author << "\n";
        cout << "Date:   " << date << "\n\n";
        cout << "    " << message << "\n\n";

        commitHash = parent;
    }
}
