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

  char keycode = ' ';
  bool request_quit = false;

  std::vector<std::function<void()>> set_select_flag;
  // 这个函数可以在没有点击该控件的情况修改他的激活状态
  // 其实就是变相记住了几个控件的地址然后可以调用内部函数
  // 下面也是，如果在view里面可以迅速获取这个地址，当然也可以，我觉得应该可以考虑这个比如在tuple里面增加name.
  std::shared_ptr<Map> map;
  std::shared_ptr<Label> label_key;
  std::shared_ptr<Label> label_position;

  glm::fvec2 map_old_position{};
  glm::ivec2 map_size{};

  int select_type=0;//0代表点绘制后面依次递增
  int special_action=0;//0代表正常绘制，-1代表擦除，-2代表选择，-3代表移动

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
