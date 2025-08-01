#include <filesystem>
#include <iostream>
#include "harbour.hpp"

namespace Harbour {
namespace Project {

bool Runner::runProject(const std::string& path) {
    ConfigManager cfg;
    if (!cfg.readConfig(path)) return false;
    std::string releaseBin = path + "/build/release/bin/" + cfg.projectName;
    std::string debugBin = path + "/build/debug/bin/" + cfg.projectName;

    bool hasRelease = std::filesystem::exists(releaseBin);
    bool hasDebug = std::filesystem::exists(debugBin);

    std::string binToRun;
    if (hasRelease && hasDebug) {
        auto tRelease = std::filesystem::last_write_time(releaseBin);
        auto tDebug = std::filesystem::last_write_time(debugBin);
        if (tDebug > tRelease) {
            binToRun = debugBin;
        } else {
            binToRun = releaseBin;
        }
    } else if (hasDebug) {
        binToRun = debugBin;
    } else if (hasRelease) {
        binToRun = releaseBin;
    } else {
        std::cerr << COLOR_RED << "No built binary found. Build the project first." << COLOR_RESET << std::endl;
        return false;
    }
    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Running " << binToRun << " ..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    Harbour::CommandExecutor exec;
    auto args = std::vector<std::string>{"/bin/sh", "-c", binToRun};
    auto result = exec.run(args, true);
    if (result.exitCode != 0) {
        debug::print("Run failed: ", result.error, result.output);
        return false;
    }
    return true;
}

} // namespace Project
} // namespace Harbour 
