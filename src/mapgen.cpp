#include "program.h"

int main(int argc, char* argv[]) {
  if(argc != 4){
    return -1;
  }
  Program program(800, 800, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
  program.run();
  return 0;
}