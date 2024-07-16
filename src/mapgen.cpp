#include "program.h"

int main(const int argc, char *argv[]) {
  if (argc != 6) {
    return -1;
  }
  // TODO
  // 我想了一个dfs的替代算法用来提高fill的时间复杂度，就是维护一个联通块的向量组，你每次使用一个符号就会生成一个该符号的联通向量，使用特定的功能会自动将你选择的点加入这个联通向量中，或者说是集合。集合直接合并就好了。然后默认是一个空格的集合，然后将里面的放到其他的符号集合里面
  // TODO 增加一个缩略图，但是不是为了拖动，因为现在修改了代码，拖动很快，但是没有预览能力。
  // 全图填充很慢，但是全图清空很快，都将在使用set之后进行优化。
  // TODO 考虑到现在的功能很多，如果继续使用旧的function向量的方式，会导致重复代码，所以直接修改成记录当前的生效的功能序号就行。
  Program program(static_cast<int>(std::strtol(argv[1], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[2], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[3], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[4], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[5], nullptr, 10)));
  program.run();
  return 0;
}
