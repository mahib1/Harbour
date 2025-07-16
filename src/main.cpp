#include "CLI.hpp"

int main(int argc, char* argv[]) {
    Harbour::Project::CLI cli;
    return cli.run(argc, argv);
}
