#pragma once
#include <string>

namespace Harbour {
namespace Project {

class Builder {
public:
    bool buildProject(const std::string& path, bool debugMode, bool cleanBuild = false);
};

} // namespace Project
} // namespace Harbour 