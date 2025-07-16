#include <iostream>
#include <string>
#include "harbour.hpp"

bool test_success() {
    std::cout << "--- Test: Command Success ---\n";
    Harbour::CommandExecutor exec;
    auto result = exec.run({"/bin/echo", "hello world"}, true);
    if (result.exitCode != 0 || result.output.find("hello world") == std::string::npos) {
        std::cerr << "FAIL: echo did not succeed or output not captured.\n";
        return false;
    }
    std::cout << "PASS: echo succeeded and output captured.\n";
    return true;
}

bool test_failure() {
    std::cout << "--- Test: Command Failure ---\n";
    Harbour::CommandExecutor exec;
    auto result = exec.run({"/bin/false"}, true);
    if (result.exitCode == 0) {
        std::cerr << "FAIL: false did not fail as expected.\n";
        return false;
    }
    std::cout << "PASS: false failed as expected.\n";
    return true;
}

int main() {
    std::cout << ">>> Running CommandExecutor Tests <<<\n\n";
    bool all_ok = true;
    all_ok &= test_success();
    all_ok &= test_failure();
    std::cout << "\n-------------------------------------\n";
    if (all_ok) {
        std::cout << ">>> All CommandExecutor tests passed successfully! <<<\n";
    } else {
        std::cout << ">>> SOME COMMANDEXECUTOR TESTS FAILED! <<<\n";
    }
    std::cout << "-------------------------------------\n";
    return all_ok ? 0 : 1;
} 
