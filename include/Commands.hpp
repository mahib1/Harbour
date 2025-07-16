#pragma once
#include <iostream>


namespace Harbour {
// Handles secure and robust command execution
class CommandExecutor {
public:
    struct Result {
        int exitCode;
        std::string output;
        std::string error;
    };
    Result run(const std::vector<std::string>& args, bool captureOutput = false);
}; 

}
