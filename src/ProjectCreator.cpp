#include "ProjectCreator.hpp"
#include "ConfigManager.hpp"
#include "colors.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>


bool ProjectCreator::createProject(const std::string& name, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics) {
    namespace fs = std::filesystem;
    try {
        fs::create_directories(name + "/include");
        fs::create_directories(name + "/src");
        fs::create_directories(name + "/external");
        std::ofstream cmake(name + "/CMakeLists.txt");
        cmake << "cmake_minimum_required(VERSION 3.16)\n";
        cmake << "project(" << name << " VERSION 1.0.0 LANGUAGES C CXX)\n\n";
        cmake << "set(CMAKE_CXX_STANDARD " << cppVersion << ")\n";
        cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
        cmake << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";
        cmake << "include_directories(include)\n\n";
        cmake << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeBin << ")\n";
        cmake << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeLib << ")\n\n";
        cmake << "set(SOURCES \n\tsrc/main.cpp\n)\n\n";
        cmake << "add_executable(" << name << " ${SOURCES})\n\n";

        if (enableGraphics) {
            cmake << "add_subdirectory(external/glfw)\n";
            cmake << "include_directories(external/glad/GL/include)\n";
            cmake << "include_directories(external/glm)\n";
            cmake << "file(GLOB GLAD_SOURCES external/glad/GL/src/gl.c)\n";
            cmake << "target_sources(" << name << " PRIVATE ${GLAD_SOURCES})\n";
            cmake << "target_link_libraries(" << name << " glfw)\n";
        }
        cmake.close();

        std::ofstream maincpp(name + "/src/main.cpp");
        maincpp << "#include \"comp.h\"\n#include <iostream>\n";

        if (enableDebug) {
            std::ofstream debughpp(name + "/include/debug.hpp");
            debughpp << "#pragma once\n#include <iostream>\n\nclass debug {\npublic:\n  template<typename... Args>\n  static void print(Args&&... args) {\n    #ifdef DEBUG\n    (std::cout << ... << args) << std::endl;\n    #endif\n  }\n};\n";
            debughpp.close();
            maincpp << "#include \"debug.hpp\"\n";
        }
        maincpp << "\nint main() {\n";

        if (enableDebug) {
            maincpp << "  debug::print(\"Test debug log\");\n";
        }

        maincpp << "  std::cout << \"Hello, world!\" << std::endl;\n  return 0;\n}\n";
        maincpp.close();

        std::ofstream comp(name + "/include/comp.h");
        comp << "// comp.h stub\n";
        comp.close();

        ConfigManager cfg;
        cfg.writeConfig(name, name, cppVersion, runtimeBin, runtimeLib, enableDebug, enableGraphics, enableGraphics ? "glfw,glad,glm" : "");

        return true;

    } catch (const std::exception& e) {
        std::cerr << COLOR_RED << "Error creating project: " << e.what() << COLOR_RESET << std::endl;
        return false;
    }
} 
