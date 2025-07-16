#include <filesystem>
#include <iostream>
#include "harbour.hpp"

namespace Harbour {
namespace Project {

bool DependencyManager::checkDependencies(bool enableGraphics) {
    if (!enableGraphics) return true;
    namespace fs = std::filesystem;
    fs::create_directories("external");
    if (!fs::exists("external/glfw")) {
        std::string cmd = "git clone https://github.com/glfw/glfw.git external/glfw";
        debug::print(cmd);
        std::cout << COLOR_YELLOW << "Cloning GLFW..." << COLOR_RESET << std::endl;
        Harbour::CommandExecutor exec;
        auto args = std::vector<std::string>{"/bin/sh", "-c", cmd};
        auto result = exec.run(args, true);
        if (result.exitCode != 0) {
            debug::print("GLFW clone failed: ", result.error, result.output);
            return false;
        }
    } else {
        std::cout << COLOR_GREEN << "GLFW found" << COLOR_RESET << std::endl;
    }
    if (!fs::exists("external/glm")) {
        std::string cmd = "git clone https://github.com/g-truc/glm.git external/glm";
        debug::print(cmd);
        std::cout << COLOR_YELLOW << "Cloning GLM..." << COLOR_RESET << std::endl;
        Harbour::CommandExecutor exec;
        auto args = std::vector<std::string>{"/bin/sh", "-c", cmd};
        auto result = exec.run(args, true);
        if (result.exitCode != 0) {
            debug::print("GLM clone failed: ", result.error, result.output);
            return false;
        }
    } else {
        std::cout << COLOR_GREEN << "GLM found" << COLOR_RESET << std::endl;
    }
    if (!fs::exists("external/glad")) {
        std::string cmd = "git clone https://github.com/Dav1dde/glad.git external/glad";
        debug::print(cmd);
        std::cout << COLOR_YELLOW << "Cloning GLAD..." << COLOR_RESET << std::endl;
        Harbour::CommandExecutor exec;
        auto args = std::vector<std::string>{"/bin/sh", "-c", cmd};
        auto result = exec.run(args, true);
        if (result.exitCode != 0) {
            debug::print("GLAD clone failed: ", result.error, result.output);
            return false;
        }
        std::string gladCmd = "cd external/glad && mkdir -p GL && python3 -m glad --out-path ./GL --api gl:compatibility=3.3 c";
        debug::print(gladCmd);
        std::cout << COLOR_YELLOW << "Running glad generator (OpenGL C 3.3 compatibility)..." << COLOR_RESET << std::endl;
        auto gladArgs = std::vector<std::string>{"/bin/sh", "-c", gladCmd};
        auto gladResult = exec.run(gladArgs, true);
        if (gladResult.exitCode != 0) {
            debug::print("GLAD generation failed: ", gladResult.error, gladResult.output);
            return false;
        }
    } else {
        std::cout << COLOR_GREEN << "GLAD found" << COLOR_RESET << std::endl;
    }
    return true;
}

} // namespace Project
} // namespace Harbour 
