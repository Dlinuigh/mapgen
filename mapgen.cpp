#include "graphic.h"
#include <cstdlib>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    exit(-1);
  }
  int w = atoi(argv[1]);
  int h = atoi(argv[2]);
  int layer = atoi(argv[3]);
  graphic grap(w, h, layer);
  SDL_Event event;
  do {
    SDL_PollEvent(&event);
    grap.begin();
    grap.draw_tool();
    grap.transfer();
    grap.draw_tool_selected();
    grap.draw_grid();
    grap.handle(event);
    grap.end();
  } while (
      grap.is_quit(event));
  grap.quit();
  return 0;
}
