#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "harbour.hpp"

const std::filesystem::path MOCK_PROJECT_ROOT = "mock_builder_project";

void cleanupMockBuilderProject() {
    std::error_code ec;
    std::filesystem::remove_all(MOCK_PROJECT_ROOT, ec);
}

void createMockConfig(const std::string& projectName) {
    std::filesystem::create_directories(MOCK_PROJECT_ROOT);
    std::ofstream file(MOCK_PROJECT_ROOT / ".harbourConfig");
    file << "project_name=\"" << projectName << "\"\n";
    file << "cpp_version=\"17\"\n";
    file << "runtime_bin=\"bin\"\n";
    file << "runtime_lib=\"lib\"\n";
    file << "enable_debug=\"false\"\n";
    file << "enable_graphics=\"false\"\n";
    file << "dependencies=\"\"\n";
    file.close();
}

bool test_build_failure() {
    std::cout << "--- Test: Build Failure (No CMakeLists.txt) ---\n";
    cleanupMockBuilderProject();
    createMockConfig("MockBuilderApp");
    Harbour::Project::Builder builder;
    bool result = builder.buildProject(MOCK_PROJECT_ROOT.string(), false, false);
    if (result) {
        std::cerr << "FAIL: Build succeeded when it should have failed.\n";
        return false;
    }
    std::cout << "PASS: Correctly failed to build without CMakeLists.txt.\n";
    return true;
}

int main() {
    std::cout << ">>> Running Builder Class Tests <<<\n\n";
    bool all_ok = true;
    all_ok &= test_build_failure();
    std::cout << "\n-------------------------------------\n";
    if (all_ok) {
        std::cout << ">>> All Builder tests passed successfully! <<<\n";
    } else {
        std::cout << ">>> SOME BUILDER TESTS FAILED! <<<\n";
    }
    std::cout << "-------------------------------------\n";
    cleanupMockBuilderProject();
    return all_ok ? 0 : 1;
} 
