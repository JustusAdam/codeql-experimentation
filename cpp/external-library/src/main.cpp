#include <iostream>

#include "lib.hpp"

void target(int input) {}

int main(int argc, char** argv) {
  std::cout << "hello from main!" << std::endl;

  int output = library_function(argc);

  target(output);
}
