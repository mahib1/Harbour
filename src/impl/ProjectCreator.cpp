#include "ProjectCreator.hpp"
#include "ConfigManager.hpp"
#include "files.hpp"
#include "colors.hpp"
#include <filesystem>
#include <iostream>

namespace Harbour {
namespace Project {

bool ProjectCreator::createProject(const std::string& name, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics) {
    namespace fs = std::filesystem;
    try {
        // Check if directory already exists
        if (fs::exists(name) && fs::is_directory(name)) {
            std::cerr << COLOR_RED << "Error: Directory '" << name << "' already exists. Project creation aborted." << COLOR_RESET << std::endl;
            return false;
        }
        fs::create_directories(name + "/include");
        fs::create_directories(name + "/src");
        fs::create_directories(name + "/external");
        {
            Harbour::FH::fileHandler cmake(name + "/CMakeLists.txt", "w");
            if (!cmake.open()) throw std::runtime_error("Failed to create CMakeLists.txt");
            auto& stream = cmake.getStream();
            stream << "cmake_minimum_required(VERSION 3.16)\n";
            stream << "project(" << name << " VERSION 1.0.0 LANGUAGES C CXX)\n\n";
            stream << "set(CMAKE_CXX_STANDARD " << cppVersion << ")\n";
            stream << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
            stream << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";
            stream << "include_directories(include)\n\n";
            stream << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeBin << ")\n";
            stream << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeLib << ")\n\n";
            stream << "set(SOURCES \n\tsrc/main.cpp\n)\n\n";
            stream << "add_executable(" << name << " ${SOURCES})\n\n";
            if (enableGraphics) {
                stream << "add_subdirectory(external/glfw)\n";
                stream << "include_directories(external/glad/GL/include)\n";
                stream << "include_directories(external/glm)\n";
                stream << "file(GLOB GLAD_SOURCES external/glad/GL/src/gl.c)\n";
                stream << "target_sources(" << name << " PRIVATE ${GLAD_SOURCES})\n";
                stream << "target_link_libraries(" << name << " glfw)\n";
            }
            cmake.close();
        }
        {
            Harbour::FH::fileHandler maincpp(name + "/src/main.cpp", "w");
            if (!maincpp.open()) throw std::runtime_error("Failed to create main.cpp");
            auto& stream = maincpp.getStream();
            stream << "#include \"comp.h\"\n#include <iostream>\n";
            if (enableDebug) {
                Harbour::FH::fileHandler debughpp(name + "/include/debug.hpp", "w");
                if (!debughpp.open()) throw std::runtime_error("Failed to create debug.hpp");
                auto& dstream = debughpp.getStream();
                dstream << "#pragma once\n#include <iostream>\n\nclass debug {\npublic:\n  template<typename... Args>\n  static void print(Args&&... args) {\n    #ifdef DEBUG\n    (std::cout << ... << args) << std::endl;\n    #endif\n  }\n};\n";
                debughpp.close();
                stream << "#include \"debug.hpp\"\n";
            }
            stream << "\nint main() {\n";
            if (enableDebug) {
                stream << "  debug::print(\"Test debug log\");\n";
            }
            stream << "  std::cout << \"Hello, world!\" << std::endl;\n  return 0;\n}\n";
            maincpp.close();
        }
        {
            Harbour::FH::fileHandler comp(name + "/include/comp.h", "w");
            if (!comp.open()) throw std::runtime_error("Failed to create comp.h");
            auto& stream = comp.getStream();
            stream << "// comp.h stub\n";
            comp.close();
        }
        ConfigManager cfg;
        cfg.writeConfig(name, name, cppVersion, runtimeBin, runtimeLib, enableDebug, enableGraphics, enableGraphics ? "glfw,glad,glm" : "");
        return true;
    } catch (const std::exception& e) {
        std::cerr << COLOR_RED << "Error creating project: " << e.what() << COLOR_RESET << std::endl;
        return false;
    }
}

} // namespace Project
} // namespace Harbour 
