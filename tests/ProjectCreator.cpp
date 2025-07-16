#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "harbour.hpp"

const std::filesystem::path MOCK_PC_ROOT = "mock_pc_project";

void cleanupMockPCProject() {
    std::error_code ec;
    std::filesystem::remove_all(MOCK_PC_ROOT, ec);
}

bool test_create_success() {
    std::cout << "--- Test: Project Creation Success ---\n";
    cleanupMockPCProject();
    Harbour::Project::ProjectCreator creator;
    bool ok = creator.createProject("mock_pc_project", 17, "bin", "lib", false, false);
    if (!ok) {
        std::cerr << "FAIL: Project creation failed.\n";
        return false;
    }
    if (!std::filesystem::exists(MOCK_PC_ROOT / "CMakeLists.txt")) {
        std::cerr << "FAIL: CMakeLists.txt not created.\n";
        return false;
    }
    std::cout << "PASS: Project created and CMakeLists.txt exists.\n";
    return true;
}

bool test_create_existing_dir() {
    std::cout << "--- Test: Project Creation with Existing Directory ---\n";
    cleanupMockPCProject();
    std::filesystem::create_directories(MOCK_PC_ROOT);
    Harbour::Project::ProjectCreator creator;
    bool ok = creator.createProject("mock_pc_project", 17, "bin", "lib", false, false);
    if (ok) {
        std::cerr << "FAIL: Project creation succeeded when it should have failed.\n";
        return false;
    }
    std::cout << "PASS: Correctly failed to create project in existing directory.\n";
    return true;
}

int main() {
    std::cout << ">>> Running ProjectCreator Tests <<<\n\n";
    bool all_ok = true;
    all_ok &= test_create_success();
    all_ok &= test_create_existing_dir();
    std::cout << "\n-------------------------------------\n";
    if (all_ok) {
        std::cout << ">>> All ProjectCreator tests passed successfully! <<<\n";
    } else {
        std::cout << ">>> SOME PROJECTCREATOR TESTS FAILED! <<<\n";
    }
    std::cout << "-------------------------------------\n";
    cleanupMockPCProject();
    return all_ok ? 0 : 1;
} 
