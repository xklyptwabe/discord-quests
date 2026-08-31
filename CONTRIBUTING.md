# Contributing to Discord Quests

Thank you for your interest in contributing to **Discord Quests**!

## Code of Conduct
Please ensure a respectful and welcoming environment for all contributors.

## How to Contribute

### 1. Reporting Bugs
- Search existing issues before creating a new one.
- Provide clear steps to reproduce the bug along with your Windows version and Discord version.

### 2. Suggesting Features
- Open an issue describing the proposed enhancement, use cases, and design ideas.

### 3. Pull Requests
1. Fork the repository and create a feature branch (`git checkout -b feat/my-feature`).
2. Follow modern C++20 conventions:
   - Use RAII for resource management.
   - Keep comments concise and written in English.
   - Adhere to the existing naming conventions (PascalCase for classes/methods, camelCase for variables, `m_` prefix for private members).
3. Ensure the project builds cleanly without compiler warnings (`/W4` on MSVC).
4. Commit using [Conventional Commits](https://www.conventionalcommits.org/) format (e.g. `feat: ...`, `fix: ...`, `refactor: ...`).
5. Open a Pull Request against `main`.

## Building the Project Locally

```powershell
# Configure CMake
cmake -B build

# Build in Release mode
cmake --build build --config Release
```
