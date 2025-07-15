#include "CLI.hpp"
#include "ConfigManager.hpp"
#include "ProjectCreator.hpp"
#include "Builder.hpp"
#include "Runner.hpp"
#include "DependencyManager.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include "colors.hpp"


int CLI::run(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <command> [options]\n";
        std::cout << "Commands:\n  new <project_name> [options]\n  build [-d] [-c|--clean] [path]\n  run [path]\n  make [-d] [-c|--clean] [path]\n";
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "new") {
        std::string projectName;
        int cppVersion = 17;
        std::string runtimeBin = "bin";
        std::string runtimeLib = "lib";
        bool enableDebug = false;
        bool enableGraphics = false;
        int i = 2;
        while (i < argc && argv[i][0] == '-') {
            std::string opt = argv[i];
            if (opt == "-v" && i + 1 < argc) {
                cppVersion = std::stoi(argv[++i]);
            } else if (opt == "-b" && i + 1 < argc) {
                runtimeBin = argv[++i];
            } else if (opt == "-l" && i + 1 < argc) {
                runtimeLib = argv[++i];
            } else if (opt == "-g") {
                enableDebug = true;
            } else if (opt == "-G") {
                enableGraphics = true;
            } else {
                std::cout << COLOR_RED << "Unknown option: " << opt << COLOR_RESET << std::endl;
                return 1;
            }
            ++i;
        }
        if (i < argc) {
            projectName = argv[i];
        } else {
            std::cout << COLOR_RED << "Project name required." << COLOR_RESET << std::endl;
            return 1;
        }
        std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Creating C++" << cppVersion << " project named " << projectName << COLOR_RESET << std::endl;
        std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
        namespace fs = std::filesystem;
        std::string projectPath = "./" + projectName;
        try {
            fs::create_directories(projectPath + "/include");
            fs::create_directories(projectPath + "/src");
            fs::create_directories(projectPath + "/external");
        } catch (const std::exception& e) {
            std::cerr << COLOR_RED << "Failed to create project directories: " << e.what() << COLOR_RESET << std::endl;
            return 1;
        }
        if (enableGraphics) {
            std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
            std::cout << COLOR_YELLOW << "Cloning graphics dependencies..." << COLOR_RESET << std::endl;
            std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
            std::string oldCwd = fs::current_path();
            fs::current_path(projectPath);
            DependencyManager dep;
            if (!dep.checkDependencies(true)) {
                std::cerr << COLOR_RED << "Failed to clone dependencies." << COLOR_RESET << std::endl;
                fs::current_path(oldCwd);
                return 1;
            }
            fs::current_path(oldCwd);
        }
        std::ofstream cmake(projectPath + "/CMakeLists.txt");
        cmake << "cmake_minimum_required(VERSION 3.16)\n";
        cmake << "project(" << projectName << " VERSION 1.0.0 LANGUAGES C CXX)\n\n";
        cmake << "set(CMAKE_CXX_STANDARD " << cppVersion << ")\n";
        cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
        cmake << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";
        cmake << "include_directories(include)\n\n";
        cmake << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeBin << ")\n";
        cmake << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeLib << ")\n\n";
        cmake << "set(SOURCES \n\tsrc/main.cpp\n)\n\n";
        cmake << "add_executable(" << projectName << " ${SOURCES})\n\n";
        if (enableGraphics) {
            cmake << "add_subdirectory(external/glfw)\n";
            cmake << "include_directories(external/glad/GL/include)\n";
            cmake << "include_directories(external/glm)\n";
            cmake << "file(GLOB GLAD_SOURCES external/glad/GL/src/gl.c)\n";
            cmake << "target_sources(" << projectName << " PRIVATE ${GLAD_SOURCES})\n";
            cmake << "target_link_libraries(" << projectName << " glfw)\n";
        }
        cmake.close();
        std::ofstream maincpp(projectPath + "/src/main.cpp");
        maincpp << "#include \"comp.h\"\n#include <iostream>\n";
        if (enableDebug) {
            std::ofstream debughpp(projectPath + "/include/debug.hpp");
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
        std::ofstream comp(projectPath + "/include/comp.h");
        comp << "// comp.h stub\n";
        comp.close();
        ConfigManager cfg;
        cfg.writeConfig(projectPath, projectName, cppVersion, runtimeBin, runtimeLib, enableDebug, enableGraphics, enableGraphics ? "glfw,glad,glm" : "");
        std::cout << COLOR_GREEN << "Project Created!" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Configuring build..." << COLOR_RESET << std::endl;
        std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
        Builder builder;
        if (!builder.buildProject(projectPath, enableDebug, true)) {
            std::cerr << COLOR_RED << "Build failed." << COLOR_RESET << std::endl;
            return 1;
        }

        std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Running program:" << COLOR_RESET << std::endl;

        Runner runner;
        runner.runProject(projectPath);
        std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
        std::cout << COLOR_GREEN << "Done!" << COLOR_RESET << std::endl;
    } else if (cmd == "build") {
        bool debugMode = false;
        bool cleanBuild = false;
        std::string buildPath = ".";
        int i = 2;
        while (i < argc && argv[i][0] == '-') {
            std::string opt = argv[i];
            if (opt == "-d") {
                debugMode = true;
            } else if (opt == "-c" || opt == "--clean") {
                cleanBuild = true;
            } else {
                std::cout << COLOR_RED << "Unknown option: " << opt << COLOR_RESET << std::endl;
                return 1;
            }
            ++i;
        }
        if (i < argc) {
            buildPath = argv[i];
        }
        Builder builder;
        if (!builder.buildProject(buildPath, debugMode, cleanBuild)) {
            std::cerr << COLOR_RED << "Build failed." << COLOR_RESET << std::endl;
            return 1;
        }
    } else if (cmd == "run") {
        std::string runPath = ".";
        if (argc > 2) {
            runPath = argv[2];
        }
        Runner runner;
        if (!runner.runProject(runPath)) {
            std::cerr << COLOR_RED << "Run failed." << COLOR_RESET << std::endl;
            return 1;
        }
    } else if (cmd == "make") {
        bool debugMode = false;
        bool cleanBuild = false;
        std::string makePath = ".";
        int i = 2;
        while (i < argc && argv[i][0] == '-') {
            std::string opt = argv[i];
            if (opt == "-d") {
                debugMode = true;
            } else if (opt == "-c" || opt == "--clean") {
                cleanBuild = true;
            } else {
                std::cout << COLOR_RED << "Unknown option: " << opt << COLOR_RESET << std::endl;
                return 1;
            }
            ++i;
        }
        if (i < argc) {
            makePath = argv[i];
        }
        Builder builder;
        if (!builder.buildProject(makePath, debugMode, cleanBuild)) {
            std::cerr << COLOR_RED << "Build failed." << COLOR_RESET << std::endl;
            return 1;
        }
        Runner runner;
        if (!runner.runProject(makePath)) {
            std::cerr << COLOR_RED << "Run failed." << COLOR_RESET << std::endl;
            return 1;
        }
    } else {
        std::cout << COLOR_RED << "Unknown command: " << cmd << COLOR_RESET << std::endl;
        return 1;
    }
    return 0;
} 
