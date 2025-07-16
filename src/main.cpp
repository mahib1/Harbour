#include "harbour.hpp"
#include "debug.hpp"

int main(int argc, char* argv[]) {
    Harbour::Project::CLI cli;
    return cli.run(argc, argv);
}
