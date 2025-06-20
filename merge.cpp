#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <map>
#include <set>
#include <queue>
#include <filesystem>
#include "helpers.hpp"

namespace fs = std::filesystem;
using namespace std;

// Parse tree lines into path → blob hash
map<string, string> parse_tree(const string &commit_hash)
{
    map<string, string> tree;
    string content = readFile(".mini-git/objects/" + commit_hash);
    istringstream stream(content);
    string line;

    // Only parse the tree object, not the commit object
    // If commit_hash is a commit, extract the tree hash and parse that object
    if (content.rfind("tree ", 0) != 0)
    {
        // This is a commit object, extract the tree hash
        istringstream commit_stream(content);
        string commit_line;
        string tree_hash;
        while (getline(commit_stream, commit_line))
        {
            if (commit_line.rfind("tree ", 0) == 0)
            {
                tree_hash = commit_line.substr(5);
                tree_hash.erase(tree_hash.find_last_not_of(" \n\r\t") + 1);
                break;
            }
        }
        if (tree_hash.empty())
            return tree;
        content = readFile(".mini-git/objects/" + tree_hash);
        stream.clear();
        stream.str(content);
    }

    while (getline(stream, line))
    {
        if (line.rfind("tree ", 0) == 0)
        {
            size_t hash_start = 5;
            size_t hash_end = line.find(" ", hash_start);
            if (hash_end == string::npos)
                continue;
            string blob_hash = line.substr(hash_start, hash_end - hash_start);
            string path = line.substr(hash_end + 1);
            if (!path.empty() && path != "tree" && path != "commit")
                tree[path] = blob_hash;
        }
    }

    return tree;
}

// Get all parent hashes from a commit file
vector<string> get_parents_from_commit(const string &commit_hash)
{
    vector<string> parents;
    string content = readFile(".mini-git/objects/" + commit_hash);
    istringstream stream(content);
    string line;

    while (getline(stream, line))
    {
        if (line.rfind("parent ", 0) == 0)
        {
            parents.push_back(line.substr(7));
        }
    }

    return parents;
}

// Find lowest common ancestor of two commits
string find_common_ancestor(const string &commit1, const string &commit2)
{
    if (commit1.empty() || commit2.empty())
        return "";

    unordered_set<string> visited;
    queue<string> q1, q2;

    q1.push(commit1);
    q2.push(commit2);

    while (!q1.empty() || !q2.empty())
    {
        if (!q1.empty())
        {
            string cur = q1.front();
            q1.pop();
            if (visited.count(cur))
                return cur;
            visited.insert(cur);
            for (auto &p : get_parents_from_commit(cur))
                q1.push(p);
        }

        if (!q2.empty())
        {
            string cur = q2.front();
            q2.pop();
            if (visited.count(cur))
                return cur;
            visited.insert(cur);
            for (auto &p : get_parents_from_commit(cur))
                q2.push(p);
        }
    }

    return "";
}
/**
 * Merges the current branch with the specified target branch.
 * It finds the common ancestor, compares trees, and resolves conflicts.
 * If successful, it creates a new commit with the merged changes.
 *
 * @param target_branch The name of the branch to merge into the current branch.
 */
void merge(const string &target_branch)
{
    string head_ref = readFile(".mini-git/HEAD");
    if (head_ref.rfind("ref: ", 0) != 0)
    {
        cout << "Detached HEAD. Cannot merge in this state.\n";
        return;
    }

    string current_branch = head_ref.substr(5);
    current_branch.erase(current_branch.find_last_not_of(" \n\r\t") + 1); // Trim whitespace

    string head_commit = readFile(".mini-git/" + current_branch);
    head_commit.erase(head_commit.find_last_not_of(" \n\r\t") + 1);

    string target_commit = readFile(".mini-git/refs/heads/" + target_branch);
    target_commit.erase(target_commit.find_last_not_of(" \n\r\t") + 1);

    if (target_commit.empty())
    {
        cout << "Branch does not exist: " << target_branch << "\n";
        return;
    }

    string base_commit = find_common_ancestor(head_commit, target_commit);
    auto base_tree = parse_tree(base_commit);
    auto our_tree = parse_tree(head_commit);
    auto their_tree = parse_tree(target_commit);

    map<string, string> result_tree;
    set<string> all_paths;

    for (const auto &[f, _] : base_tree)
        all_paths.insert(f);
    for (const auto &[f, _] : our_tree)
        all_paths.insert(f);
    for (const auto &[f, _] : their_tree)
        all_paths.insert(f);

    bool conflict = false;
    map<string, string> files_to_write;

    for (const auto &path : all_paths)
    {
        string base = base_tree.count(path) ? base_tree[path] : "";
        string ours = our_tree.count(path) ? our_tree[path] : "";
        string theirs = their_tree.count(path) ? their_tree[path] : "";

        if (ours == theirs)
        {
            if (!ours.empty())
                result_tree[path] = ours;
        }
        else if (base == ours)
        {
            result_tree[path] = theirs;
        }
        else if (base == theirs)
        {
            result_tree[path] = ours;
        }
        else
        {
            cout << "CONFLICT: " << path << "\n";
            conflict = true;
            continue;
        }

        files_to_write[path] = result_tree[path];
    }

    if (conflict)
    {
        cout << "Automatic merge failed. Resolve conflicts and commit manually.\n";
        return;
    }

    // Only write files and index if no conflicts
    ofstream index_out(".mini-git/index");
    for (const auto &[path, hash] : files_to_write)
    {
        ifstream blob(".mini-git/objects/" + hash, ios::binary);
        if (!blob)
        {
            cerr << "Missing blob: " << hash << "\n";
            continue;
        }
        ofstream out_file(path, ios::binary);
        if (!out_file)
        {
            cerr << "Failed to write file: " << path << "\n";
            continue;
        }
        out_file << blob.rdbuf();
        index_out << path << " " << hash << "\n";
    }
    index_out.close();

    // Step 5: Create new tree and commit
    stringstream tree_content;
    for (const auto &[path, hash] : result_tree)
        tree_content << "tree " << hash << " " << path << "\n";

    string tree_hash = compute_hash(tree_content.str());
    ofstream tree_file(".mini-git/objects/" + tree_hash);
    if (!tree_file)
    {
        cerr << "Failed to write tree object.\n";
        return;
    }
    tree_file << tree_content.str();

    stringstream commit_content;
    commit_content << "tree " << tree_hash << "\n";
    commit_content << "parent " << head_commit;
    commit_content << "\nparent " << target_commit << "\n";
    commit_content << "author " << get_author_data() << "\n";
    commit_content << "date " << get_timestamp() << "\n";
    commit_content << "message Merged branch " << target_branch << "\n";

    string commit_str = commit_content.str();
    string commit_hash = compute_hash(commit_str);
    ofstream commit_file(".mini-git/objects/" + commit_hash);
    if (!commit_file)
    {
        cerr << "Failed to write commit object.\n";
        return;
    }
    commit_file << commit_str;

    // Update HEAD
    ofstream branch_out(".mini-git/" + current_branch);
    if (!branch_out)
    {
        cerr << "Failed to update branch ref.\n";
        return;
    }
    branch_out << commit_hash;

    ofstream clear_index(".mini-git/index");
    clear_index.close();

    cout << "Merge successful. New commit: " << commit_hash << "\n";
}
