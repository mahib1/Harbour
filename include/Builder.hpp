#pragma once
#include <string>

class Builder {
public:
    bool buildProject(const std::string& path, bool debugMode, bool cleanBuild = false);
}; 