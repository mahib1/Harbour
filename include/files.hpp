#pragma once

#include <cstddef>
#include <filesystem> // Use the filesystem library
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>

namespace Harbour {
namespace FH {

typedef enum {
  READ = 0,
  WRITE = 1,
  APPEND = 2,
} FILE_MODE;

class fileHandler {
private:
  std::fstream file;
  std::filesystem::path filePath; // Use std::filesystem::path
  std::vector<Harbour::FH::FILE_MODE> modes;

public:
  fileHandler(const std::filesystem::path &path, const std::string &modeStr) {
    this->filePath = path;
    for (char i : modeStr) {
      switch (i) {
      case 'r':
        modes.push_back(READ);
        break;
      case 'w':
        modes.push_back(WRITE);
        break;
      case 'a':
        modes.push_back(APPEND);
        break;
      default:
        throw std::invalid_argument("Invalid character in mode string.");
      }
    }
  }

  ~fileHandler() {
    if (file.is_open()) {
      file.close();
    }
  }

  bool open() {
    std::ios_base::openmode final_mode = {};
    bool has_read = false, has_write = false, has_append = false;

    // 1. Determine which modes were requested
    for (auto f : this->modes) {
      if (f == READ)
        has_read = true;
      if (f == WRITE)
        has_write = true;
      if (f == APPEND)
        has_append = true;
    }

    // 2. If writing/appending, ensure the path exists first
    if (has_write || has_append) {
      if (filePath.has_parent_path()) {
        const auto parentDir = filePath.parent_path();
        if (!std::filesystem::exists(parentDir)) {
          std::filesystem::create_directories(parentDir);
        }
      }
      // "Touch" the file to create it if it doesn't exist, without truncating
      std::ofstream touch(filePath, std::ios::app);
      touch.close();
    }

    // 3. Build the final mode flags based on the user's intent
    if (has_append) {
      final_mode |= std::ios::app | std::ios::out;
      if (has_read)
        final_mode |= std::ios::in;
    } else if (has_write) {
      final_mode |= std::ios::out;
      if (has_read) {
        // "rw" mode: NO truncation
        final_mode |= std::ios::in;
      } else {
        // "w" only mode: YES truncation
        final_mode |= std::ios::trunc;
      }
    } else if (has_read) {
      final_mode |= std::ios::in;
    }

    // 4. Open the file with the correctly built mode
    file.open(filePath, final_mode);
    return file.is_open();
  }

  void close() {
    if (file.is_open()) {
      file.close();
    }
  }

  void writeLines(const std::vector<std::string> &lines) {
    if (!file.is_open() || !file.good())
      throw std::runtime_error("File is not open for writing.");
    for (const auto &line : lines) {
      file << line << '\n';
    }
  }

  void seekToByte(size_t offset) {
    if (!file.is_open() || !file.good())
      throw std::runtime_error("File is not open for seeking.");
    file.seekg(offset, std::ios::beg);
    if (file.fail())
      throw std::runtime_error("Seek operation failed.");
  }

  void jumpToLine(size_t lineNumber) {
    if (!file.is_open() || !file.good())
      throw std::runtime_error("File is not open for seeking.");
    file.seekg(0, std::ios::beg);
    file.clear();
    if (lineNumber == 1)
      return;
    std::string temp_line;
    for (size_t i = 0; i < lineNumber - 1; ++i) {
      if (!std::getline(file, temp_line)) {
        throw std::runtime_error(
            "Cannot jump to line; file does not have that many lines.");
      }
    }
  }

  void jumpToEnd() {
    if (!file.is_open() || !file.good())
      throw std::runtime_error("File is not open for seeking.");
    file.clear();
    file.seekg(0, std::ios::end);
    if (file.fail())
      throw std::runtime_error("Seek to end operation failed.");
  }

  std::fstream &getStream() { return file; }
  const std::filesystem::path &getPath() const { return filePath; }
};

} // namespace FH
} // namespace Harbour
