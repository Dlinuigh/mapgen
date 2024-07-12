#include "graphic.h"
#include <SDL3/SDL_events.h>
#include <cstdlib>

int main(int argc, char *argv[]) {
  // FIXME 定义100x100将会导致需要的屏幕非常大，应该设计成随意窗口大小、使用鼠标中键可以移动视图、使用右键可以呼出画笔选择
  Graphic graphic;
  SDL_Event event;
  do {
    SDL_PollEvent(&event);
    graphic.run();
    graphic.draw();
  } while (
      grap.is_quit(event));
  grap.quit();
  return 0;
}
