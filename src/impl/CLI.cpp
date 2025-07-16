#include "CLI.hpp"
#include "Builder.hpp"
#include "ConfigManager.hpp"
#include "DependencyManager.hpp"
#include "ProjectCreator.hpp"
#include "Runner.hpp"
#include "colors.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace Harbour {
namespace Project {

int CLI::run(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <command> [options]\n";
    std::cout << "Commands:\n  new <project_name> [options]\n  build [-d] "
                 "[-c|--clean] [path]\n  run [path]\n  make [-d] [-c|--clean] "
                 "[path]\n";
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

    // Parse options
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
        std::cerr << COLOR_RED << "Unknown option: " << opt << COLOR_RESET
                  << std::endl;
        return 1;
      }
      ++i;
    }

    // Get project name
    if (i < argc) {
      projectName = argv[i];
    } else {
      std::cerr << COLOR_RED << "Project name required." << COLOR_RESET
                << std::endl;
      return 1;
    }

    std::cout << COLOR_MAGENTA
              << "=============================================================================" << COLOR_RESET
              << std::endl;
    std::cout << COLOR_YELLOW << "Creating C++" << cppVersion
              << " project named " << projectName << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA
              << "=============================================================================" << COLOR_RESET
              << std::endl;

    ProjectCreator creator;
    if (!creator.createProject(projectName, cppVersion, runtimeBin, runtimeLib,
                               enableDebug, enableGraphics)) {
      std::cerr << COLOR_RED << "Project creation failed." << COLOR_RESET
                << std::endl;
      return 1;
    }

    std::cout << COLOR_GREEN << "Project Created!" << COLOR_RESET << std::endl;

    // Clone dependencies if needed
    if (enableGraphics) {
      std::cout << COLOR_YELLOW << "Cloning graphics dependencies..."
                << COLOR_RESET << std::endl;
      std::string oldCwd = std::filesystem::current_path();
      std::filesystem::current_path("./" + projectName);
      DependencyManager dep;
      if (!dep.checkDependencies(true)) {
        std::cerr << COLOR_RED << "Failed to clone dependencies." << COLOR_RESET
                  << std::endl;
        std::filesystem::current_path(oldCwd);
        return 1;
      }
      std::filesystem::current_path(oldCwd);
    }

    std::cout << COLOR_YELLOW << "Configuring build..." << COLOR_RESET
              << std::endl;
    Builder builder;
    if (!builder.buildProject("./" + projectName, enableDebug, true)) {
      std::cerr << COLOR_RED << "Build failed." << COLOR_RESET << std::endl;
      return 1;
    }

    std::cout << COLOR_YELLOW << "Running program:" << COLOR_RESET << std::endl;
    Runner runner;
    runner.runProject("./" + projectName);

    std::cout << COLOR_MAGENTA
              << "=============================================================================" << COLOR_RESET
              << std::endl;
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
        std::cout << COLOR_RED << "Unknown option: " << opt << COLOR_RESET
                  << std::endl;
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
        std::cout << COLOR_RED << "Unknown option: " << opt << COLOR_RESET
                  << std::endl;
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
    std::cout << COLOR_RED << "Unknown command: " << cmd << COLOR_RESET
              << std::endl;
    return 1;
  }
  return 0;
}

} // namespace Project
} // namespace Harbour
