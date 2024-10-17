#include "lib.hpp"

#include <iostream>

__attribute__((noinline))
int hidden_function(int input) {
  std::cout << "hello from library function!" << std::endl;
  return input;
}

int library_function(int input) {
  return hidden_function(input);
}
