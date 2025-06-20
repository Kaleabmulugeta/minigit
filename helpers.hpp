#include <iostream>
#include <map>
#include <vector>
using namespace std;
string get_current_commit();
string get_timestamp();
std::string compute_hash(std::string data);
string get_author_data(void);
string readFile(const string path);
bool fileExists(const string &path);
std::string save_object(std::string path_to_file);
string trim(const string &s);
void writeFile(const string &path, const string &content);
string getTreeHashFromCommit(const string &commitContent);
map<string, string> parseTreeObject(const string &treeContent);
vector<string> getModifiedFiles(const map<string, string> &committedFiles);
map<string, string> getCurrentTrackedFiles();
string read_index();
string generate_tree(string &current_commit_hash);
string get_commit_parent(string &content);
void update_current_branch(const string &commitHash);
string readBlobContent(const string &blobHash);
bool check_mod(const string &path_to_file);