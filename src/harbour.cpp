#include "harbour.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>
#include "debug.hpp"

// ANSI color codes
const char* COLOR_RESET   = "\033[0m";
const char* COLOR_RED     = "\033[31m";
const char* COLOR_GREEN   = "\033[32m";
const char* COLOR_YELLOW  = "\033[33m";
const char* COLOR_MAGENTA = "\033[35m";

bool ConfigManager::readConfig(const std::string& path) {
    std::ifstream infile(path + "/.harbourConfig");
    if (!infile) {
        std::cerr << COLOR_RED << ".harbourConfig not found in " << path << COLOR_RESET << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(infile, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        if (!value.empty() && value.front() == '"') value = value.substr(1, value.size() - 2);
        if (key == "project_name") projectName = value;
        else if (key == "cpp_version") cppVersion = std::stoi(value);
        else if (key == "runtime_bin") runtimeBin = value;
        else if (key == "runtime_lib") runtimeLib = value;
        else if (key == "enable_debug") enableDebug = (value == "true");
        else if (key == "enable_graphics") enableGraphics = (value == "true");
        else if (key == "dependencies") dependencies = value;
    }
    return true;
}

bool ConfigManager::writeConfig(const std::string& path, const std::string& projectName, int cppVersion, const std::string& runtimeBin, const std::string& runtimeLib, bool enableDebug, bool enableGraphics, const std::string& dependencies) {
    std::ofstream outfile(path + "/.harbourConfig");
    if (!outfile) return false;
    outfile << "project_name=\"" << projectName << "\"\n";
    outfile << "cpp_version=\"" << cppVersion << "\"\n";
    outfile << "runtime_bin=\"" << runtimeBin << "\"\n";
    outfile << "runtime_lib=\"" << runtimeLib << "\"\n";
    outfile << "enable_debug=\"" << (enableDebug ? "true" : "false") << "\"\n";
    outfile << "enable_graphics=\"" << (enableGraphics ? "true" : "false") << "\"\n";
    outfile << "dependencies=\"" << dependencies << "\"\n";
    return true;
}

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
        std::ofstream maincpp(name + "/src/main.cpp");
        maincpp << "#include \"comp.h\"\n#include <iostream>\n";
        if (enableDebug) {
            std::ofstream debughpp(name + "/include/debug.hpp");
            debughpp << "#pragma once\n#include <iostream>\n\nclass debug {\npublic:\n  template<typename... Args>\n  static void print(Args&&... args) {\n    #ifdef DEBUG\n    (std::cout << ... << args) << std::endl;\n    #endif\n  }\n};\n";
            maincpp << "#include \"debug.hpp\"\n";
        }
        maincpp << "\nint main() {\n";
        if (enableDebug) {
            maincpp << "  debug::print(\"Test debug log\");\n";
        }
        maincpp << "  std::cout << \"Hello, world!\" << std::endl;\n  return 0;\n}\n";
        std::ofstream comp(name + "/include/comp.h");
        comp << "// comp.h stub\n";
        ConfigManager cfg;
        cfg.writeConfig(name, name, cppVersion, runtimeBin, runtimeLib, enableDebug, enableGraphics, enableGraphics ? "glfw,glad,glm" : "");
        return true;
    } catch (const std::exception& e) {
        std::cerr << COLOR_RED << "Error creating project: " << e.what() << COLOR_RESET << std::endl;
        return false;
    }
}

bool DependencyManager::checkDependencies(bool enableGraphics) {
    if (!enableGraphics) return true;
    namespace fs = std::filesystem;
    fs::create_directories("external");
    if (!fs::exists("external/glfw")) {
        std::string cmd = "git clone https://github.com/glfw/glfw.git external/glfw";
        debug::print(cmd);
        std::cout << COLOR_YELLOW << "Cloning GLFW..." << COLOR_RESET << std::endl;
        if (system(cmd.c_str()) != 0) return false;
    } else {
        std::cout << COLOR_GREEN << "GLFW found" << COLOR_RESET << std::endl;
    }
    if (!fs::exists("external/glm")) {
        std::string cmd = "git clone https://github.com/g-truc/glm.git external/glm";
        debug::print(cmd);
        std::cout << COLOR_YELLOW << "Cloning GLM..." << COLOR_RESET << std::endl;
        if (system(cmd.c_str()) != 0) return false;
    } else {
        std::cout << COLOR_GREEN << "GLM found" << COLOR_RESET << std::endl;
    }
    if (!fs::exists("external/glad")) {
        std::string cmd = "git clone https://github.com/Dav1dde/glad.git external/glad";
        debug::print(cmd);
        std::cout << COLOR_YELLOW << "Cloning GLAD..." << COLOR_RESET << std::endl;
        if (system(cmd.c_str()) != 0) return false;
        std::string gladCmd = "cd external/glad && mkdir -p GL && python3 -m glad --out-path ./GL --api gl:compatibility=3.3 c";
        debug::print(gladCmd);
        std::cout << COLOR_YELLOW << "Running glad generator (OpenGL C 3.3 compatibility)..." << COLOR_RESET << std::endl;
        if (system(gladCmd.c_str()) != 0) return false;
    } else {
        std::cout << COLOR_GREEN << "GLAD found" << COLOR_RESET << std::endl;
    }
    return true;
}

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
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Checking for dependencies..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    DependencyManager dep;
    if (!dep.checkDependencies(cfg.enableGraphics)) return false;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Configuring build..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::string absProjectRoot = fs::absolute(path);
    std::string cmakeCmd = "cd '" + buildPath + "' && cmake '" + absProjectRoot + "'";
    if (debugMode) {
        std::cout << COLOR_YELLOW << "Debug mode enabled" << COLOR_RESET << std::endl;
        cmakeCmd += " -DCMAKE_CXX_FLAGS=\"-DDEBUG\"";
    }
    debug::print(cmakeCmd);
    if (system(cmakeCmd.c_str()) != 0) return false;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Building project..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::string makeCmd = "cd '" + buildPath + "' && make";
    debug::print(makeCmd);
    if (system(makeCmd.c_str()) != 0) return false;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Moving binary to src/..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::string binPath = buildPath + "/" + cfg.runtimeBin + "/" + cfg.projectName;
    std::string destPath = path + "/src/" + cfg.projectName + (debugMode ? "_debug" : "");
    debug::print(binPath);
    if (fs::exists(binPath)) {
        fs::copy_file(binPath, destPath, fs::copy_options::overwrite_existing);
        std::cout << COLOR_GREEN << "Moved " << cfg.projectName << (debugMode ? "_debug" : "") << " to src/" << COLOR_RESET << std::endl;
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
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "Done! Run it via ./src/" << cfg.projectName << (debugMode ? "_debug" : "") << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    return true;
}

bool Runner::runProject(const std::string& path) {
    ConfigManager cfg;
    if (!cfg.readConfig(path)) return false;
    std::string releaseBin = path + "/src/" + cfg.projectName;
    std::string debugBin = path + "/src/" + cfg.projectName + "_debug";
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
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Running " << binToRun << " ..." << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "=======================================" << COLOR_RESET << std::endl;
    int ret = system(binToRun.c_str());
    return ret == 0;
}

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
        // If graphics enabled, clone dependencies first
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
        // Write CMakeLists.txt
        std::ofstream cmake(projectPath + "/CMakeLists.txt");
        cmake << "cmake_minimum_required(VERSION 3.16)\n";
        cmake << "project(" << projectName << " VERSION 1.0.0 LANGUAGES C CXX)\n\n";
        cmake << "set(CMAKE_CXX_STANDARD " << cppVersion << ")\n";
        cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
        cmake << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";
        cmake << "include_directories(include)\n\n";
        cmake << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeBin << ")\n";
        cmake << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/" << runtimeLib << ")\n\n";
        cmake << "set(SOURCES src/main.cpp)\n\n";
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
        // Write main.cpp
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
        // Write config
        ConfigManager cfg;
        cfg.writeConfig(projectPath, projectName, cppVersion, runtimeBin, runtimeLib, enableDebug, enableGraphics, enableGraphics ? "glfw,glad,glm" : "");
        std::cout << COLOR_GREEN << "Project Created!" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Configuring build..." << COLOR_RESET << std::endl;
        std::cout << COLOR_MAGENTA << "==========================================" << COLOR_RESET << std::endl;
        // Add a short delay to allow filesystem to sync
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

int main(int argc, char* argv[]) {
    CLI cli;
    return cli.run(argc, argv);
} 