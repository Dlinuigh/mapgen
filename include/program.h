#ifndef MAPGEN_H
#define MAPGEN_H
#include "font.h"
#include "graphic.h"
#include "view.h"
#include <SDL3/SDL.h>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
class Program {
  SDL_Event event;
  SDL_Window *window;
  SDL_Renderer *render;
  SDL_Texture *view;
  std::shared_ptr<View> v_main;
  glm::ivec2 scr_size;
  Graphic &graphic;
  Font &font;
  char code = ' ';
  bool request_quit = false;
  std::function<void(char)> set_key;
  std::function<void(std::vector<bool>)> set_function;
  std::vector<std::function<void()>> set_select_flag;
  glm::ivec2 map_size;
  std::vector<bool> function = {false, false, false, false};
  void create_map();
  void create_v_main();
  void handle();
  void print();
  void trigger(int);
  void draw();
  bool is_quit(SDL_Event);
  void init();
  void set_view();

public:
  Program(int w, int h, int col, int row)
      : graphic(Graphic::getInstance()), font(Font::getInstance()) {
    scr_size = glm::ivec2(w, h);
    init();
    set_view();
    create_v_main();
    map_size = glm::ivec2(col, row);
    create_map();
  }
  void run();
  ~Program() {
    SDL_DestroyTexture(view);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};
#endif