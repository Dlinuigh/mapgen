#include "program.h"

int main(const int argc, char *argv[]) {
  if (argc != 6) {
    return -1;
  }
  // TODO
  // 程序的内存占用应该不会是大问题，测试过250x250,表现非常优秀，一点也不卡。然后显示上很清晰。接下来可以的是，加入设置功能，加入绘制复杂图形的功能，加入预设图案，比如设计好几种房间后可以一键添加。获取预设然后粘贴。加入多层的地图绘制，使用<>键可以切换地图的层级，然后将底层进行虚化，或者虚化顶层，和pixelorama一样，基本上我的工作可以以pixelorama为参考，只是我的做法更加diy更加艰苦。
  // TODO 首先解决内存泄漏的问题，然后再去开发1. 自由绘制 2. 画圆 3. 画框
  // 我一次性添加了7个功能，预计还有，随机化功能，上下分层功能。这两个功能实现之前，我需要对代码进行一次整理，变量名顺序之类的修改。
  // TODO
  // 我想了一个dfs的替代算法用来提高fill的时间复杂度，就是维护一个联通块的向量组，你每次使用一个符号就会生成一个该符号的联通向量，使用特定的功能会自动将你选择的点加入这个联通向量中，或者说是集合。集合直接合并就好了。然后默认是一个空格的集合，然后将里面的放到其他的符号集合里面
  // TODO 增加一个缩略图，但是不是为了拖动，因为现在修改了代码，拖动很快，但是没有预览能力。
  // 全图填充很慢，但是全图清空很快，都将在使用set之后进行优化。
  Program program(static_cast<int>(std::strtol(argv[1], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[2], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[3], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[4], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[5], nullptr, 10)));
  program.run();
  return 0;
}
