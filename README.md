# Mini-git

> A minimal version control system inspired by Git, written in C++

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/std/the-standard)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Cross--Platform-lightgrey.svg)](https://github.com/Kaleabmulugeta/mini-git)

A lightweight, educational version control system that mimics Git's core functionality. Perfect for learning how VCS works under the hood! 🔍

---

## ✨ Features

- 🏗️ **Repository Management** - Initialize, track, and manage projects
- 📝 **Commit System** - Stage files and create commits with messages
- 🌿 **Branching** - Create and switch between branches
- 📋 **Logging** - View commit history and metadata
- 🔍 **Diff Tool** - Compare changes between commits
- 💾 **Content-Addressed Storage** - SHA1-based object storage

---

## 🛠️ Quick Start

### Prerequisites
- C++17 compiler (g++, clang++)
- OpenSSL library

### Build & Run
```bash
# Compile
g++ -std=c++17 -o mini-git *.cpp -lssl -lcrypto

# Initialize repository
./mini-git init

# Add files
./mini-git add <filename>

# Commit changes
./mini-git commit -m "Initial commit"

# View history
./mini-git log
```

---

## 📚 Usage Examples

| Command | Description |
|---------|-------------|
| `./mini-git init` | Initialize new repository |
| `./mini-git add <file>` | Stage file for commit |
| `./mini-git commit -m "msg"` | Commit staged changes |
| `./mini-git branch <name>` | Create new branch |
| `./mini-git checkout <branch>` | Switch to branch |
| `./mini-git log` | View commit history |
| `./mini-git diff <id1> <id2>` | Compare commits |

---

## 🏗️ Architecture

```
.mini-git/
├── objects/         # Blobs, trees, commits (SHA1)
├── refs/heads/      # Branch pointers
├── HEAD            # Current branch/commit
├── index           # Staging area
└── config          # Author info
```

### Core Objects
- **Blob** - File content storage
- **Tree** - Directory snapshots
- **Commit** - Project state records
- **Branch** - Development line pointers

---

## 🎯 Learning Goals

This project demonstrates:
- Content-addressed storage systems
- Tree-based file organization
- Commit graph traversal
- Branch management
- Staging area concepts

Perfect for understanding Git internals! 🧠

---

## 🔮 Roadmap

- [ ] Merge implementation
- [ ] Remote repository support
- [ ] Interactive CLI
- [ ] Line-by-line diff output
- [ ] Unit test suite
- [ ] Performance optimizations

---

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request


<div align="center">

**Built with ❤️ for learning version control systems**

[⭐ Star this repo](https://github.com/Kaleabmulugeta/mini-git) • [🐛 Report issues](https://github.com/Kaleabmulugeta/mini-git/issues) 

</div> 
