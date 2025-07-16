#pragma once
#include <string>

namespace Harbour {
namespace Project {

class ConfigManager {
public:
    bool readConfig(const std::string& path);
    bool writeConfig(const std::string& path, const std::string& projectName, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics, const std::string& dependencies);
    std::string projectName;
    int cppVersion;
    std::string runtimeBin;
    std::string runtimeLib;
    bool enableDebug;
    bool enableGraphics;
    std::string dependencies;
};

} // namespace Project
} // namespace Harbour 