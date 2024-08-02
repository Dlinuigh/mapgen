#include "program.h"

int main(const int argc, char *argv[]) {
  if (argc != 6) {
    return -1;
  }
  Program program(static_cast<int>(std::strtol(argv[1], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[2], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[3], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[4], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[5], nullptr, 10)));
  program.run();
  return 0;
}
