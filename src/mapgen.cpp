#include "program.h"

int main(const int argc, char *argv[]) {
  if (argc != 6) {
    return -1;
  }
  // TODO 增加一个缩略图，但是不是为了拖动，因为现在修改了代码，拖动很快，但是没有预览能力。
  // 全图填充很慢，但是全图清空很快，都将在使用set之后进行优化。
  Program program(static_cast<int>(std::strtol(argv[1], nullptr, 10)), static_cast<int>(std::strtol(argv[2], nullptr, 10)), static_cast<int>(std::strtol(argv[3], nullptr, 10)), static_cast<int>(std::strtol(argv[4], nullptr, 10)), static_cast<int>(std::strtol(argv[5], nullptr, 10)));
  program.run();
  return 0;
}
