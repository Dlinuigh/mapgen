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
  SDL_Event event{};
  SDL_Window *window{};
  SDL_Renderer *render{};
  SDL_Texture *view{};
  std::shared_ptr<View> v_main;
  glm::ivec2 scr_size{};
  float tile_size;
  Graphic &graphic;
  Font &font;
  char code = ' ';
  bool request_quit = false;
  std::function<void(char)> set_key;
  std::function<void(std::vector<bool>)> set_function;
  std::vector<std::function<void()>> set_select_flag;
  std::shared_ptr<Map> map;
  std::shared_ptr<Label> label_key;
  std::shared_ptr<Label> label_position;
  glm::fvec2 right_button_position{};
  bool right_button_down = false;
  glm::fvec2 map_old_position{};
  glm::ivec2 map_size{};
  std::vector<bool> function = {false, false, false, false};

  void create_map();

  void create_v_main();

  void handle();

  void print() const;

  void trigger(int);

  void draw() const;

  static bool is_quit(const SDL_Event &);

  void init();

  void set_view();

public:
  Program(const int w, const int h, const int col, const int row,
          const int _tile_size)
      : tile_size(static_cast<float>(_tile_size)),
        graphic(Graphic::getInstance()), font(Font::getInstance()) {
    scr_size = glm::ivec2(w, h);
    init();
    set_view();
    map_size = glm::ivec2(col, row);
    create_v_main();
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
