#include <chrono> // For std::chrono::milliseconds
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread> // For std::this_thread::sleep_for

#include "harbour.hpp"

const std::filesystem::path MOCK_PROJECT_ROOT = "mock_project";

void cleanupMockProject() {
  std::error_code ec;
  std::filesystem::remove_all(MOCK_PROJECT_ROOT, ec);
}

void createDirs(const std::string &type) {
  std::filesystem::create_directories(MOCK_PROJECT_ROOT / "build" / type /
                                      "bin");
}

void createFakeBinary(const std::string &type, bool should_succeed) {
  createDirs(type);
  std::filesystem::path binPath =
      MOCK_PROJECT_ROOT / "build" / type / "bin" / "MockApp";
  std::ofstream file(binPath);
  file << "#!/bin/sh\n";
  file << "echo \"--- Running the " << type << " binary ---\"\n";
  file << "exit " << (should_succeed ? "0" : "1") << "\n";
  file.close();

  std::filesystem::permissions(binPath,
                               std::filesystem::perms::owner_all |
                                   std::filesystem::perms::group_exec |
                                   std::filesystem::perms::others_exec,
                               std::filesystem::perm_options::add);
}

void createConfigFile(const std::string &projectName) {
  std::ofstream file(MOCK_PROJECT_ROOT / ".harbourConfig");

  file << "{\"projectName\": \"" << projectName << "\"}";
  file.close();
}


bool test_no_binaries() {
  std::cout << "--- Test: No Binaries Found ---\n";
  cleanupMockProject();
  createConfigFile("MockApp");

  Harbour::Project::Runner runner;
  bool result = runner.runProject(MOCK_PROJECT_ROOT.string());

  if (result) { // Should fail
    std::cerr << "FAIL: Succeeded when no binaries exist.\n";
    return false;
  }
  std::cout << "PASS: Correctly failed when no binaries were found.\n";
  return true;
}

bool test_run_release_only() {
  std::cout << "--- Test: Release Only ---\n";
  cleanupMockProject();
  createConfigFile("MockApp");
  createFakeBinary("release", true);

  Harbour::Project::Runner runner;
  if (!runner.runProject(MOCK_PROJECT_ROOT.string())) {
    std::cerr << "FAIL: Failed to run the release binary.\n";
    return false;
  }
  std::cout << "PASS: Correctly ran the release binary.\n";
  return true;
}

bool test_run_newer_debug() {
  std::cout << "--- Test: Newer Debug Binary ---\n";
  cleanupMockProject();
  createConfigFile("MockApp");
  createFakeBinary("release", true);
  // Wait briefly to ensure a different last_write_time
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  createFakeBinary("debug", true);

  Harbour::Project::Runner runner;
  if (!runner.runProject(MOCK_PROJECT_ROOT.string())) {
    std::cerr << "FAIL: Failed to run the newer debug binary.\n";
    return false;
  }
  std::cout << "PASS: Correctly chose and ran the newer debug binary.\n";
  return true;
}

bool test_run_failure() {
  std::cout << "--- Test: Binary Execution Failure ---\n";
  cleanupMockProject();
  createConfigFile("MockApp");
  createFakeBinary("release", false); // This binary will exit with code 1

  Harbour::Project::Runner runner;
  if (runner.runProject(MOCK_PROJECT_ROOT.string())) { // Should fail
    std::cerr << "FAIL: runProject succeeded even when the binary failed.\n";
    return false;
  }
  std::cout << "PASS: Correctly reported failure for a failing binary.\n";
  return true;
}

int main() {
  std::cout << ">>> Running Runner Class Tests <<<\n\n";
  bool all_ok = true;

  all_ok &= test_no_binaries();
  all_ok &= test_run_release_only();
  all_ok &= test_run_newer_debug();
  all_ok &= test_run_failure();

  std::cout << "\n-------------------------------------\n";
  if (all_ok) {
    std::cout << ">>> All Runner tests passed successfully! <<<\n";
  } else {
    std::cout << ">>> SOME RUNNER TESTS FAILED! <<<\n";
  }
  std::cout << "-------------------------------------\n";

  cleanupMockProject();
  return all_ok ? 0 : 1;
}
