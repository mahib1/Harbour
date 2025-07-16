#pragma once

#include <cstddef>
#include <filesystem>
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
  std::filesystem::path filePath;
  std::vector<Harbour::FH::FILE_MODE> modes;

public:
  fileHandler(const std::filesystem::path &path, const std::string &modeStr);
  ~fileHandler();
  bool open();
  void close();
  void writeLines(const std::vector<std::string> &lines);
  void seekToByte(size_t offset);
  void jumpToLine(size_t lineNumber);
  void jumpToEnd();
  std::fstream &getStream();
  const std::filesystem::path &getPath() const;
};

} // namespace FH
} // namespace Harbour
