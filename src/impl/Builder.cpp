#include <filesystem>
#include <iostream>
#include "harbour.hpp"

namespace Harbour {
namespace Project {

bool Builder::buildProject(const std::string& path, bool debugMode, bool cleanBuild) {
    namespace fs = std::filesystem;
    std::string buildType = debugMode ? "debug" : "release";
    std::string buildPath = path + "/build/" + buildType;

    if (cleanBuild && fs::exists(buildPath)) {
        std::string rmCmd = "rm -rf " + buildPath;
        debug::print(rmCmd);
        fs::remove_all(buildPath);
    }

    fs::create_directories(buildPath);

    ConfigManager cfg;
    if (!cfg.readConfig(path)) return false;

    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Checking for dependencies..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;

    DependencyManager dep;
    if (!dep.checkDependencies(cfg.enableGraphics)) return false;

    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Configuring build..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::string absProjectRoot = fs::absolute(path);
    std::string cmakeCmd = "cd '" + buildPath + "' && cmake '" + absProjectRoot + "'";

    if (debugMode) {
        std::cout << COLOR_YELLOW << "Debug mode enabled" << COLOR_RESET << std::endl;
        cmakeCmd += " -DCMAKE_CXX_FLAGS=\"-DDEBUG\"";
    }
    debug::print(cmakeCmd);

    Harbour::CommandExecutor exec;
    auto cmakeArgs = std::vector<std::string>{"/bin/sh", "-c", cmakeCmd};
    auto cmakeResult = exec.run(cmakeArgs, true);
    if (cmakeResult.exitCode != 0) {
        debug::print("CMake failed: ", cmakeResult.error, cmakeResult.output);
        return false;
    }

    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Building project..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::string makeCmd = "cd '" + buildPath + "' && make";

    debug::print(makeCmd);

    auto makeArgs = std::vector<std::string>{"/bin/sh", "-c", makeCmd};
    auto makeResult = exec.run(makeArgs, true);
    if (makeResult.exitCode != 0) {
        debug::print("Make failed: ", makeResult.error, makeResult.output);
        return false;
    }

    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Linking the Compile Commands for clangd" << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::string binPath = buildPath + "/" + cfg.runtimeBin + "/" + cfg.projectName;

    debug::print(binPath);

    if (fs::exists(binPath)) {
        // Copy compile_commands.json to project root for clangd
        std::string compileCommandsSrc = buildPath + "/compile_commands.json";
        std::string compileCommandsDst = path + "/compile_commands.json";

        if (fs::exists(compileCommandsSrc)) {
            fs::copy_file(compileCommandsSrc, compileCommandsDst, fs::copy_options::overwrite_existing);
            std::cout << COLOR_GREEN << "Copied compile_commands.json to project root." << COLOR_RESET << std::endl;
        }

    } else {
        std::cerr << COLOR_RED << "Build did not produce expected binary: " << cfg.projectName << COLOR_RESET << std::endl;
        return false;
    }


    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "Done! Run it via 'harbour run'" << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "==========================================================================" << COLOR_RESET << std::endl;

    return true;
}

} // namespace Project
} // namespace Harbour 
