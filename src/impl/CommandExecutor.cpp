#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sstream>
#include "harbour.hpp"

namespace Harbour {

CommandExecutor::Result CommandExecutor::run(const std::vector<std::string>& args, bool captureOutput) {
    int outPipe[2], errPipe[2];
    pid_t pid;
    std::ostringstream outStream, errStream;
    int status = -1;

    if (captureOutput) {
        pipe(outPipe);
        pipe(errPipe);
    }

    pid = fork();
    if (pid == 0) {
        if (captureOutput) {
            dup2(outPipe[1], STDOUT_FILENO);
            dup2(errPipe[1], STDERR_FILENO);
            close(outPipe[0]); close(outPipe[1]);
            close(errPipe[0]); close(errPipe[1]);
        }
        std::vector<char*> cargs;
        for (const auto& s : args) cargs.push_back(const_cast<char*>(s.c_str()));
        cargs.push_back(nullptr);
        execvp(cargs[0], cargs.data());
        _exit(127);
    } else if (pid > 0) {
        if (captureOutput) {
            close(outPipe[1]);
            close(errPipe[1]);
            char buf[4096];
            ssize_t n;
            while ((n = read(outPipe[0], buf, sizeof(buf))) > 0) outStream.write(buf, n);
            while ((n = read(errPipe[0], buf, sizeof(buf))) > 0) errStream.write(buf, n);
            close(outPipe[0]);
            close(errPipe[0]);
        }
        waitpid(pid, &status, 0);
    } else {
        debug::print("fork() failed");
        return {127, "", "fork() failed"};
    }
    int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : status;
    if (exitCode != 0) debug::print("Command failed with code ", exitCode);
    return {exitCode, outStream.str(), errStream.str()};
}

} // namespace Harbour 
