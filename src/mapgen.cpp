#include "program.h"

int main(int argc, char* argv[]) {
  Program program(800, 600, atoi(argv[1]), atoi(argv[2]));
  program.run();
}