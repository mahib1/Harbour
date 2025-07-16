#include "ConfigManager.hpp"
#include "files.hpp"
#include <iostream>
#include "colors.hpp"

namespace Harbour {
namespace Project {

bool ConfigManager::readConfig(const std::string& path) {
    Harbour::FH::fileHandler infile(path + "/.harbourConfig", "r");
    if (!infile.open()) {
        std::cerr << COLOR_RED << ".harbourConfig not found in " << path << COLOR_RESET << std::endl;
        return false;
    }
    std::string line;
    auto& stream = infile.getStream();
    while (std::getline(stream, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        if (!value.empty() && value.front() == '"') value = value.substr(1, value.size() - 2);
        if (key == "project_name") projectName = value;
        else if (key == "cpp_version") cppVersion = std::stoi(value);
        else if (key == "runtime_bin") runtimeBin = value;
        else if (key == "runtime_lib") runtimeLib = value;
        else if (key == "enable_debug") enableDebug = (value == "true");
        else if (key == "enable_graphics") enableGraphics = (value == "true");
        else if (key == "dependencies") dependencies = value;
    }
    infile.close();
    return true;
}

bool ConfigManager::writeConfig(const std::string& path, const std::string& projectName, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics, const std::string& dependencies) {
    Harbour::FH::fileHandler outfile(path + "/.harbourConfig", "w");
    if (!outfile.open()) return false;
    auto& stream = outfile.getStream();
    stream << "project_name=\"" << projectName << "\"\n";
    stream << "cpp_version=\"" << cppVersion << "\"\n";
    stream << "runtime_bin=\"" << runtimeBin << "\"\n";
    stream << "runtime_lib=\"" << runtimeLib << "\"\n";
    stream << "enable_debug=\"" << (enableDebug ? "true" : "false") << "\"\n";
    stream << "enable_graphics=\"" << (enableGraphics ? "true" : "false") << "\"\n";
    stream << "dependencies=\"" << dependencies << "\"\n";
    outfile.close();
    return true;
}

} // namespace Project
} // namespace Harbour 
