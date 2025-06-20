#include <iostream>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <stdexcept>
#include <fstream>
#include <openssl/sha.h>

using namespace std;
namespace fs = filesystem;

string read_index()
{
    ifstream indexFile(".mini-git/index");
    stringstream ss;
    string line;
    while (getline(indexFile, line))
    {
        istringstream iss(line);
        string filename, hash;
        iss >> filename >> hash;
        ss << "blob " << hash << " " << filename << "\n";
    }
    return ss.str();
}

void update_current_branch(const string &commitHash)
{
    ifstream head(".mini-git/HEAD");
    string ref;
    getline(head, ref);
    if (ref.rfind("ref: ", 0) == 0)
    {
        string refPath = ref.substr(5);
        ofstream branchFile(".mini-git/" + refPath);
        branchFile << commitHash;
    }
}
string get_commit_parent(string &content)
{
    istringstream lines(content);
    string line;

    while (getline(lines, line))
    {
        if (line.find("parent ", 0) == 0)
            return line.substr(7);
    }
    return "null";
}

string readFile(const string path)
{
    ifstream file(path);
    if (!file)
        return "";
    try
    {
        return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    }
    catch (const exception &e)
    {
        cerr << "readFile " << path << '\n';
        return "";
    }
}
string generate_tree(string &current_commit_hash)
{
    map<string, string> latest_blobs; // path -> blob hash

    // Step 1: Walk commit history and collect previous tree entries
    string commit_hash = current_commit_hash;
    while (!commit_hash.empty() && commit_hash != "null")
    {
        string commit_path = ".mini-git/objects/" + commit_hash;
        {
            if (!fs::exists(commit_path))
                break;

            string commit_content = readFile(commit_path);
            istringstream stream(commit_content);
            string line;

            while (getline(stream, line))
            {
                if (line.rfind("tree ", 0) == 0)
                {
                    // Format: tree <blob-hash> <path>
                    size_t hash_start = 5;
                    size_t hash_end = line.find(" ", hash_start);
                    string hash = line.substr(hash_start, hash_end);
                    string file = readFile(".mini-git/objects/" + hash);
                    string path, tmp1, tmp2;
                    istringstream splitter(file);
                    splitter >> tmp1 >> tmp2 >> path;
                    cout << path << "\n";

                    if (latest_blobs.count(path) == 0)
                    {
                        latest_blobs[path] = hash;
                    }
                }
            }

            commit_hash = get_commit_parent(commit_content);
        }
    }

    // Step 2: Override with staged entries from index
    ifstream indexFile(".mini-git/index");
    string line;
    while (getline(indexFile, line))
    {
        istringstream iss(line);
        string path, hash;
        iss >> path >> hash;

        if (!path.empty() && !hash.empty())
        {
            latest_blobs[path] = hash;
        }
    }

    // Step 3: Format as full tree snapshot
    stringstream result;
    for (const auto &entry : latest_blobs)
    {
        result << "tree " << entry.second << " " << entry.first << "\n";
    }

    return result.str();
}

string get_current_commit()
{
    ifstream head(".mini-git/HEAD");
    string ref;
    getline(head, ref);
    if (ref.rfind("ref: ", 0) == 0)
    {
        string refPath = ref.substr(5);
        ifstream branchFile(".mini-git/" + refPath);
        string commitHash;
        getline(branchFile, commitHash);
        return commitHash;
    }
    return "";
}
string get_timestamp()
{
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    ostringstream ss;
    ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

string get_author_data(void)
{
    ifstream config(".mini-git/config");
    string line, name, email;

    while (getline(config, line))
    {
        if (line.find("name = ") == 0)
        {
            name = line.substr(7); // skip "name = "
        }
        else if (line.find("email = ") == 0)
        {
            email = line.substr(8); // skip "email = "
        }
    }

    if (!name.empty() && !email.empty())
    {
        return name + " <" + email + ">";
    }
    else
    {
        return "Unknown <unknown@example.com>";
    }
}

string compute_hash(string data)
{
    unsigned char ret[20];
    SHA1(reinterpret_cast<const unsigned char *>(data.c_str()), data.size(), ret);
    ostringstream hex;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        hex << std::hex << std::setw(2) << std::setfill('0') << (int)ret[i];
    return hex.str();
}

string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \n\r\t");
    size_t end = s.find_last_not_of(" \n\r\t");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}
string save_object(string path_to_file)
{
    ifstream file(path_to_file);
    if (file.is_open())
    {
        string line;
        ostringstream content_stream;
        while (getline(file, line))
        {
            content_stream << line << "\n";
        }
        file.close();
        string content = trim(content_stream.str());
        string hash = compute_hash(content);
        if (fs::exists(".mini-git/objects/" + hash))
            return hash;
        ofstream file(".mini-git/objects/" + hash);
        if (!file)
        {
            cout << "Error adding file\n";
            return "";
        }
        file << content;
        file.close();
        return hash;
    }
    else
        return "";
}

bool fileExists(const string &path)
{
    return fs::exists(path);
}

void writeFile(const string &path, const string &content)
{
    fs::path filePath(path);
    if (filePath.has_parent_path())
    {
        fs::create_directories(filePath.parent_path());
    }
    ofstream file(path);
    if (!file)
    {
        throw runtime_error("Failed to write to file: " + path);
    }
    file << content;
}

string getTreeHashFromCommit(const string &commitContent)
{
    istringstream ss(commitContent);
    string line;
    while (getline(ss, line))
    {
        if (line.find("tree ", 0) == 0)
        {
            return trim(line.substr(5));
        }
    }
    return "";
}
// Extract clean file content from blob (handle both styles)
string readBlobContent(const string &blobHash)
{
    string raw = readFile(".minigit/objects/" + blobHash);
    return raw;
}
map<string, string> parseTreeObject(const string &treeContent)
{
    map<string, string> fileMap; // filename -> blobHash
    istringstream ss(treeContent);
    string line;
    while (getline(ss, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        string type, hash, filename;
        iss >> type >> hash >> filename;
        fileMap[filename] = hash;
    }
    return fileMap;
}

vector<string> getModifiedFiles(const map<string, string> &committedFiles)
{
    vector<string> modifiedFiles;
    for (const auto &[filename, blobHash] : committedFiles)
    {
        if (!fileExists(filename))
        {
            modifiedFiles.push_back(filename + " (deleted)");
            continue;
        }
        string currentContent = readFile(filename);
        string blobContent = readFile(".mini-git/objects/" + blobHash);
        if (currentContent != blobContent)
        {
            modifiedFiles.push_back(filename + " (modified)");
        }
    }
    return modifiedFiles;
}

map<string, string> getCurrentTrackedFiles()
{
    string headContent = readFile(".mini-git/HEAD");
    string commitHash;
    if (headContent.find("ref: ") == 0)
    {
        string refPath = ".mini-git/" + trim(headContent.substr(5));
        commitHash = trim(readFile(refPath));
    }
    else
    {
        commitHash = trim(headContent); // Detached HEAD
    }

    string commitContent = readFile(".mini-git/objects/" + commitHash);
    string treeHash = getTreeHashFromCommit(commitContent);
    if (treeHash.empty())
        return {};

    string treeContent = readFile(".mini-git/objects/" + treeHash);
    return parseTreeObject(treeContent); // filename -> blobHash
}

bool check_mod(const string &path_to_file)
{
    string latestCommit = get_current_commit();
    if (latestCommit.empty())
        return true;
    string commitContent = readFile(".mini-git/objects/" + latestCommit);
    string treeHash = getTreeHashFromCommit(commitContent);
    if (treeHash.empty())
        return true;
    string blob_path;
    istringstream treeStream(readFile(".mini-git/objects/" + treeHash));
    string line;
    while (getline(treeStream, line))
    {
        if (line.find("tree ", 0) == 0)
        {
            istringstream iss(line);
            string type, hash, filename;
            iss >> type >> hash >> filename;
            if ((filename) == path_to_file)
            {
                blob_path = hash;
                break;
            }
        }
    }
    if (blob_path.empty())
    {
        return true; // File not found in the latest commit, consider it modified
    }
    else
    {
        string currentContent = trim(readFile(path_to_file));
        string blobContent = trim(readFile(".mini-git/objects/" + blob_path));
        return currentContent != blobContent; // Check if content differs
    }
}
