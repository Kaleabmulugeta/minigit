#include <iostream>

void add_file(std::string path_to_file);
void branch(const std::string &branchName);
void commit(const std::string &message);
void log(void);
void initialize_repo(void);
void checkout(const std::string &ref, bool force = false);
void merge(const std::string &target_branch);
void diffCommits(const std::string &commitHash1, const std::string &commitHash2);