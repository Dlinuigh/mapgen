#include "program.h"

int main(int argc, char* argv[]) {
  if(argc != 3){
    return -1;
  }
  Program program(800, 600, atoi(argv[1]), atoi(argv[2]));
  program.run();
  return 0;
}