#pragma once
#include <string>

namespace Harbour {
namespace Project {

class ProjectCreator {
public:
    bool createProject(const std::string& name, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics);
};

} // namespace Project
} // namespace Harbour 