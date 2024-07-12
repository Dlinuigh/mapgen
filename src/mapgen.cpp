#include "graphic.h"
#include "view.h"
#include "widget.h"
#include <SDL3/SDL.h>
#include <memory>
class Mapgen {
  SDL_Event event;
  SDL_Window *window;
  SDL_Renderer *render;
  SDL_Texture *view;
  std::shared_ptr<View> v_main;
  glm::ivec2 scr_size;
  Graphic &graphic;
  bool request_quit = false;
  std::vector<std::function<void()>> set_select_flag;
  void create_v_main() {
    v_main = std::make_shared<View>(scr_size);
    auto point = std::make_shared<Check>(graphic.get_tile("tool", "point"));
    auto rect = std::make_shared<Check>(graphic.get_tile("tool", "rect"));
    auto eraser = std::make_shared<Check>(graphic.get_tile("tool", "eraser"));
    auto bucket = std::make_shared<Check>(graphic.get_tile("tool", "bucket"));
    auto save = std::make_shared<Check>(graphic.get_tile("tool", "save"));
    point->set_check_texture(graphic.get_tile("tool", "select"));
    rect->set_check_texture(graphic.get_tile("tool", "select"));
    eraser->set_check_texture(graphic.get_tile("tool", "select"));
    bucket->set_check_texture(graphic.get_tile("tool", "select"));
    glm::fvec2 enlarge_tile(64, 64);
    point->set_desire_size(enlarge_tile);
    rect->set_desire_size(enlarge_tile);
    eraser->set_desire_size(enlarge_tile);
    bucket->set_desire_size(enlarge_tile);
    save->set_desire_size(enlarge_tile);
    point->callback = std::bind(&Mapgen::trigger, this, 0);
    rect->callback = std::bind(&Mapgen::trigger, this, 1);
    eraser->callback = std::bind(&Mapgen::trigger, this, 2);
    bucket->callback = std::bind(&Mapgen::trigger, this, 3);
    set_select_flag.push_back(std::bind(&Check::activate, point));
    set_select_flag.push_back(std::bind(&Check::activate, rect));
    set_select_flag.push_back(std::bind(&Check::activate, eraser));
    set_select_flag.push_back(std::bind(&Check::activate, bucket));
    auto panel = std::make_shared<Box>(false);
    panel->push_back(point);
    panel->push_back(rect);
    panel->push_back(eraser);
    panel->push_back(bucket);
    panel->push_back(save);
    panel->set_size();
    v_main->push_back(panel, U_SIDE);
    v_main->locate();
  }
  void handle() {
    SDL_PollEvent(&event);
    if (event.type == SDL_EVENT_KEY_DOWN) {
      // 由当前的view决定当前的快捷键
      for (auto it : v_main->shotcut) {
        if (it.first == event.key.key) {
          it.second();
        }
      }
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      // 点击事件由当前的view决定，使得通过添加一个view可以快速实现下拉菜单
      v_main->click();
    } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
      // 更新画布的大小
      scr_size.x = event.window.data1;
      scr_size.y = event.window.data2;
      set_view();
      v_main->set_screen_size(scr_size);
      v_main->locate();
    } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
      request_quit = true;
    }
  }
  std::vector<bool> function = {false, false, false, false};
  void trigger(int idx) {
    switch (idx) {
    case 0: {
      if (function[1])
        function[1].flip();
      if(function[3])
        function[3].flip();
      function[0].flip();
      break;
    }
    case 1: {
      if (function[0])
        function[0].flip();
      if(function[3])
        function[3].flip();
      function[1].flip();
      break;
    }
    case 2: {
      function[2].flip();
      break;
    }
    case 3: {
      if (function[1])
        function[1].flip();
      if(function[0])
        function[0].flip();
      function[3].flip();
      break;
    }
    default:;
    }
    for(int i=0;i<4;i++){
      if(function[i])
        set_select_flag[i]();
    }
  }
  void draw() {
    SDL_SetRenderTarget(render, view);
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderClear(render);
    v_main->draw(render, event);
    SDL_SetRenderTarget(render, nullptr);
    SDL_RenderTexture(render, view, nullptr, nullptr);
    SDL_RenderPresent(render);
  }
  bool is_quit(SDL_Event event) {
    return (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_Q &&
            event.key.mod & SDL_KMOD_CTRL);
  }
  void init() {
    window = SDL_CreateWindow("Mapgen", scr_size.x, scr_size.y,
                              SDL_WINDOW_RESIZABLE);
    render = SDL_CreateRenderer(window, nullptr);
  }
  void set_view() {
    if (view != nullptr) {
      SDL_DestroyTexture(view);
    }
    view = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
                             SDL_TEXTUREACCESS_TARGET, scr_size.x, scr_size.y);
  }

public:
  void run() {
    do {
      handle();
      draw();
    } while (!is_quit(event) && !request_quit);
  }
  Mapgen(int w, int h) : graphic(Graphic::getInstance()) {
    scr_size = glm::ivec2(w, h);
    init();
    set_view();
    create_v_main();
  }
  ~Mapgen() {
    SDL_DestroyTexture(view);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};

int main() {
  Mapgen mapgen(800, 600);
  mapgen.run();
}