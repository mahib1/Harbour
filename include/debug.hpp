#pragma once
#include <iostream>

class debug {
public:
  template<typename... Args>
  static void print(Args&&... args) {
    #ifdef DEBUG
    (std::cout << ... << args) << std::endl;
    #endif
  }
};
