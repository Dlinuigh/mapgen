#include "program.h"

int main(int argc, char* argv[]) {
  if(argc != 6){
    return -1;
  }
  // TODO 程序的内存占用应该不会是大问题，测试过250x250,表现非常优秀，一点也不卡。然后显示上很清晰。接下来可以的是，加入设置功能，加入绘制复杂图形的功能，加入预设图案，比如设计好几种房间后可以一键添加。获取预设然后粘贴。加入多层的地图绘制，使用<>键可以切换地图的层级，然后将底层进行虚化，或者虚化顶层，和pixelorama一样，基本上我的工作可以以pixelorama为参考，只是我的做法更加diy更加艰苦。
  Program program(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
  program.run();
  return 0;
}