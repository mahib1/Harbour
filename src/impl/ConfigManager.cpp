#include "ConfigManager.hpp"
#include <fstream>
#include <iostream>
#include "colors.hpp"

namespace Harbour {
namespace Project {

bool ConfigManager::readConfig(const std::string& path) {
    std::ifstream infile(path + "/.harbourConfig");
    if (!infile) {
        std::cerr << COLOR_RED << ".harbourConfig not found in " << path << COLOR_RESET << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(infile, line)) {
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
    return true;
}

bool ConfigManager::writeConfig(const std::string& path, const std::string& projectName, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics, const std::string& dependencies) {
    std::ofstream outfile(path + "/.harbourConfig");
    if (!outfile) return false;
    outfile << "project_name=\"" << projectName << "\"\n";
    outfile << "cpp_version=\"" << cppVersion << "\"\n";
    outfile << "runtime_bin=\"" << runtimeBin << "\"\n";
    outfile << "runtime_lib=\"" << runtimeLib << "\"\n";
    outfile << "enable_debug=\"" << (enableDebug ? "true" : "false") << "\"\n";
    outfile << "enable_graphics=\"" << (enableGraphics ? "true" : "false") << "\"\n";
    outfile << "dependencies=\"" << dependencies << "\"\n";
    return true;
}

} // namespace Project
} // namespace Harbour 
