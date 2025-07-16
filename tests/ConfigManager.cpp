#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "harbour.hpp"

const std::filesystem::path MOCK_CONFIG_ROOT = "mock_config_project";

void cleanupMockConfigProject() {
    std::error_code ec;
    std::filesystem::remove_all(MOCK_CONFIG_ROOT, ec);
}

bool test_read_missing() {
    std::cout << "--- Test: Read Missing Config ---\n";
    cleanupMockConfigProject();
    Harbour::Project::ConfigManager cfg;
    bool result = cfg.readConfig(MOCK_CONFIG_ROOT.string());
    if (result) {
        std::cerr << "FAIL: Read succeeded when config was missing.\n";
        return false;
    }
    std::cout << "PASS: Correctly failed to read missing config.\n";
    return true;
}

bool test_write_and_read() {
    std::cout << "--- Test: Write and Read Config ---\n";
    cleanupMockConfigProject();
    std::filesystem::create_directories(MOCK_CONFIG_ROOT);
    Harbour::Project::ConfigManager cfg;
    bool writeOk = cfg.writeConfig(MOCK_CONFIG_ROOT.string(), "TestApp", 17, "bin", "lib", true, false, "");
    if (!writeOk) {
        std::cerr << "FAIL: Could not write config.\n";
        return false;
    }
    Harbour::Project::ConfigManager cfg2;
    bool readOk = cfg2.readConfig(MOCK_CONFIG_ROOT.string());
    if (!readOk || cfg2.projectName != "TestApp" || cfg2.cppVersion != 17) {
        std::cerr << "FAIL: Could not read config or values incorrect.\n";
        return false;
    }
    std::cout << "PASS: Wrote and read config successfully.\n";
    return true;
}

int main() {
    std::cout << ">>> Running ConfigManager Tests <<<\n\n";
    bool all_ok = true;
    all_ok &= test_read_missing();
    all_ok &= test_write_and_read();
    std::cout << "\n-------------------------------------\n";
    if (all_ok) {
        std::cout << ">>> All ConfigManager tests passed successfully! <<<\n";
    } else {
        std::cout << ">>> SOME CONFIGMANAGER TESTS FAILED! <<<\n";
    }
    std::cout << "-------------------------------------\n";
    cleanupMockConfigProject();
    return all_ok ? 0 : 1;
} 
