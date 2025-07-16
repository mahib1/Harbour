#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "harbour.hpp"


const std::filesystem::path MOCK_DEP_ROOT = "mock_dep_project";

void cleanupMockDepProject() {
    std::error_code ec;
    std::filesystem::remove_all(MOCK_DEP_ROOT, ec);
}

void createMockDepDir() {
    std::filesystem::create_directories(MOCK_DEP_ROOT);
    std::filesystem::current_path(MOCK_DEP_ROOT);
}

bool test_no_graphics() {
    std::cout << "--- Test: No Graphics Dependencies ---\n";
    cleanupMockDepProject();
    createMockDepDir();
    Harbour::Project::DependencyManager dep;
    bool result = dep.checkDependencies(false);
    if (!result) {
        std::cerr << "FAIL: checkDependencies(false) failed.\n";
        return false;
    }
    std::cout << "PASS: checkDependencies(false) succeeded.\n";
    std::filesystem::current_path("../");
    return true;
}

int main() {
    std::cout << ">>> Running DependencyManager Class Tests <<<\n\n";
    bool all_ok = true;
    all_ok &= test_no_graphics();
    std::cout << "\n-------------------------------------\n";
    if (all_ok) {
        std::cout << ">>> All DependencyManager tests passed successfully! <<<\n";
    } else {
        std::cout << ">>> SOME DEPENDENCYMANAGER TESTS FAILED! <<<\n";
    }
    std::cout << "-------------------------------------\n";
    cleanupMockDepProject();
    return all_ok ? 0 : 1;
} 
