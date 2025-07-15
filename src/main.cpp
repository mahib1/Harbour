#include "comp.h"
#include <iostream>
#include "debug.hpp"

int main() {
  debug::print("Test debug log");
  std::cout << "Hello, world!" << std::endl;
  return 0;
}
