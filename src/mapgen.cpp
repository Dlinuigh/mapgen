#include "program.h"

int main(const int argc, char *argv[]) {
  if (argc != 6) {
    return -1;
  }
  // TODO
  // 程序的内存占用应该不会是大问题，测试过250x250,表现非常优秀，一点也不卡。然后显示上很清晰。接下来可以的是，加入设置功能，加入绘制复杂图形的功能，加入预设图案，比如设计好几种房间后可以一键添加。获取预设然后粘贴。加入多层的地图绘制，使用<>键可以切换地图的层级，然后将底层进行虚化，或者虚化顶层，和pixelorama一样，基本上我的工作可以以pixelorama为参考，只是我的做法更加diy更加艰苦。
  // FIXME 内存似乎一直在增加，可能发生了内存泄漏
  Program program(static_cast<int>(std::strtol(argv[1], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[2], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[3], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[4], nullptr, 10)),
                  static_cast<int>(std::strtol(argv[5], nullptr, 10)));
  program.run();
  return 0;
}
