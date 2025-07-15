#pragma once
#include <string>
#include <vector>

// Handles reading and writing .harbourConfig
class ConfigManager {
public:
    bool readConfig(const std::string& path);
    bool writeConfig(const std::string& path, const std::string& projectName, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics, const std::string& dependencies);
    // Config values
    std::string projectName;
    int cppVersion;
    std::string runtimeBin;
    std::string runtimeLib;
    bool enableDebug;
    bool enableGraphics;
    std::string dependencies;
};

// Handles project creation logic
class ProjectCreator {
public:
    bool createProject(const std::string& name, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics);
};

// Handles build logic
class Builder {
public:
    bool buildProject(const std::string& path, bool debugMode, bool cleanBuild = false);
};

// Handles running the built binary
class Runner {
public:
    bool runProject(const std::string& path);
};

// Handles dependency checking and cloning
class DependencyManager {
public:
    bool checkDependencies(bool enableGraphics);
};

// Handles CLI parsing and dispatch
class CLI {
public:
    int run(int argc, char* argv[]);
}; 