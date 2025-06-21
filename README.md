# mini-git

A minimal version control system inspired by Git, written in C++. This project provides basic version control features such as repository initialization, file tracking, committing, branching, logging, and more. The goal is to help users understand the core concepts behind version control systems by providing a simplified, transparent implementation.

---

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Directory Structure](#directory-structure)
- [Internal Design & Implementation](#internal-design--implementation)
  - [Object Types](#object-types)
  - [Command Workflows](#command-workflows)
  - [Utility & Helper Functions](#utility--helper-functions)
- [Glossary](#glossary)
- [Future Improvements](#future-improvements)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Notes](#notes)
- [License](#license)

---

## Overview

**mini-git** is a lightweight, educational version control system (VCS) that mimics the basic functionality of Git. It is implemented in C++ and stores all repository data in a hidden `.mini-git` directory. The project is designed for learning and experimentation, making it easy to see how core VCS concepts like commits, branches, and file tracking are implemented under the hood.

---

## Features
- Initialize a new repository
- Add files to staging
- Commit changes with messages
- View commit logs
- Create and switch branches
- Checkout branches or specific commits
- Merge branches (planned)
- Show differences between commits

---

## Directory Structure

When you initialize a repository, the following structure is created:

```
.mini-git/
├── objects/         # Stores file blobs, trees, and commit objects (by SHA1 hash)
├── refs/
│   └── heads/       # Stores branch pointers (one file per branch)
├── HEAD             # Points to the current branch or commit
├── index            # Staging area for files to be committed
└── config           # Stores author information
```

- **objects/**: Contains all content-addressed objects (blobs, trees, commits).
- **refs/heads/**: Each file is a branch, containing the latest commit hash for that branch.
- **HEAD**: A file that points to the current branch (e.g., `ref: refs/heads/master`) or a specific commit (detached HEAD).
- **index**: The staging area, listing files and their hashes to be included in the next commit.
- **config**: Stores the author's name and email.

---

## Internal Design & Implementation

### Object Types

- **Blob**: Stores the contents of a single file. Named by the SHA1 hash of its content.
- **Tree**: Represents a snapshot of the project at a commit. Lists all tracked files and their blob hashes.
- **Commit**: Contains metadata (parent commit, author, date, message, tree hash) and points to a tree object.
- **Branch Reference**: A file in `refs/heads/` that stores the latest commit hash for a branch.

### Command Workflows

#### `init`
- Creates the `.mini-git` directory and all subdirectories.
- Initializes `HEAD` to point to `refs/heads/master`.
- Prompts the user for author name and email, storing them in `config`.
- Creates an empty `index` file for staging.

#### `add <filename>`
- Checks if the file exists and is modified since the last commit.
- Reads the file, computes its SHA1 hash, and stores its content as a blob in `objects/` (if not already present).
- Appends the filename and hash to the `index` file (staging area).
- Prevents adding unmodified files to avoid redundant storage.

#### `commit -m "<message>"`
- Reads the current index and previous commit.
- Generates a tree object representing the current state of tracked files (merging previous tree and staged changes).
- Creates a commit object with:
  - Tree hash
  - Parent commit hash
  - Author (from config)
  - Timestamp
  - Commit message
- Stores the commit object in `objects/`.
- Updates the current branch pointer to the new commit.
- Clears the index (staging area).

#### `branch <branch-name>`
- Reads the current commit hash from `HEAD`.
- Creates a new file in `refs/heads/` named after the branch, containing the current commit hash.
- Prevents overwriting existing branches.

#### `checkout <branch-name | commitID>`
- Resolves the target branch or commit.
- Checks for uncommitted changes or untracked files that would be overwritten.
- Updates the working directory to match the target commit's tree (removes, updates, or adds files as needed).
- Updates `HEAD` to point to the new branch or commit.
- Supports forced checkout to override local changes.

#### `log`
- Reads the current branch's commit hash from `HEAD`.
- Traverses the commit history by following parent hashes.
- Prints each commit's hash, author, date, and message in reverse chronological order.

#### `diff <commitID1> <commitID2>`
- Loads the tree objects for both commits.
- Compares the file lists and hashes, reporting:
  - Added files
  - Removed files
  - Modified files
  - Unchanged files
- Does not show line-by-line differences (future improvement).

#### `merge <branch-name>` (Planned)
- Would merge the specified branch into the current branch, handling conflicts and updating the commit history.

### Utility & Helper Functions
- **Filesystem Operations**: Uses C++17 `<filesystem>` for cross-platform file and directory management.
- **SHA1 Hashing**: Uses OpenSSL to compute hashes for content-addressed storage.
- **Helpers**: Functions for reading/writing files, computing hashes, parsing trees, checking file modification status, and managing the staging area.
- **Error Handling**: Basic error messages for missing files, invalid operations, and repository state issues.

---

## Glossary
- **Blob**: The raw content of a file, stored by its hash.
- **Tree**: A snapshot of the project, mapping filenames to blob hashes.
- **Commit**: A record of a project state, with metadata and a pointer to a tree.
- **Branch**: A movable pointer to a commit, representing a line of development.
- **HEAD**: The current branch or commit checked out in the working directory.
- **Index**: The staging area for files to be included in the next commit.
- **Detached HEAD**: When `HEAD` points directly to a commit, not a branch.

---

## Future Improvements
- **Merge Implementation**: Add a true merge algorithm with conflict detection and resolution.
- **Status Command**: Show staged, modified, and untracked files.
- **Remote Repositories**: Add push/pull/fetch for remote collaboration.
- **Better Diff Output**: Show line-by-line differences, not just file status.
- **Configurable Author Info**: Allow changing author info after init.
- **Improved Error Handling**: More robust checks and user feedback.
- **Unit Tests**: Add automated tests for all commands and helpers.
- **Performance**: Optimize for large repositories and files.
- **Cross-Platform Support**: Ensure compatibility with Windows, Linux, and macOS.
- **Hooks and Extensions**: Allow user-defined scripts to run on certain actions (e.g., pre-commit, post-commit).
- **Interactive CLI**: Add a more user-friendly command-line interface with help and tab completion.
- **Documentation**: Expand with diagrams, code samples, and tutorials.

---

## Getting Started

### Prerequisites
- C++17 compatible compiler (e.g., g++, clang++)
- OpenSSL library (for SHA1 hashing)

### Build Instructions

1. Clone or download this repository.
2. Compile the source files:
   ```sh
   g++ -std=c++17 -o mini-git main.cpp addFiles.cpp branch.cpp checkout.cpp commit.cpp diff.cpp init.cpp log.cpp utils.cpp -lssl -lcrypto
   ```
3. Run the program:
   ```sh
   ./mini-git <command> [options]
   ```

---

## Usage

### Initialize a Repository
```
./mini-git init
```
Prompts for author name and email, and creates a `.mini-git` directory.

### Add a File
```
./mini-git add <filename>
```
Stages a file for the next commit. Only modified files are added.

### Commit Changes
```
./mini-git commit -m "<message>"
```
Records staged changes with a commit message. Updates the branch pointer and clears the staging area.

### View Commit Log
```
./mini-git log
```
Shows the history of commits, including hash, author, date, and message.

### Create a Branch
```
./mini-git branch <branch-name>
```
Creates a new branch at the current commit. Branches are lightweight pointers.

### Checkout a Branch or Commit
```
./mini-git checkout <branch-name | commitID>
```
Switches to the specified branch or commit. Updates the working directory to match the target state.

### Merge a Branch
```
./mini-git merge <branch-name>
```
Merges the specified branch into the current branch (planned feature).

### Show Differences Between Commits
```
./mini-git diff <commitID1> <commitID2>
```
Displays file changes between two commits (added, removed, modified, unchanged).

---

## Notes
- All repository data is stored in the `.mini-git` directory.
- Only basic error handling is implemented.
- This project is for educational purposes and is not intended for production use.

