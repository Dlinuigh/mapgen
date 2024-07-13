#include "program.h"
void Program::create_map() {
  auto map = std::make_shared<Box>(true);
  for (int i = 0; i < map_size.x; i++) {
    auto box = std::make_shared<Box>(false);
    for (int j = 0; j < map_size.y; j++) {
      auto cell = std::make_shared<Cell>('c');
      cell->callback = std::bind(&Cell::set_value, cell, key);
      cell->font = font.get_font("Terminus.ttf", 16);
      cell->fcolor = {0,0,0,255};
      cell->set_desire_size(glm::fvec2(24,24));
      box->push_back(cell);
    }
    box->set_size();
    map->push_back(box);
  }
  map->set_size();
  v_main->push_back(map, CENTER);
  v_main->locate();
}
void Program::create_v_main() {
  // TODO 我想要增加一个label或者entry以支持当前字符显示和直接输入字符
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
  point->callback = std::bind(&Program::trigger, this, 0);
  rect->callback = std::bind(&Program::trigger, this, 1);
  eraser->callback = std::bind(&Program::trigger, this, 2);
  bucket->callback = std::bind(&Program::trigger, this, 3);
  save->callback = std::bind(&Program::print, this);
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
  // 整理大小
  panel->set_size();
  v_main->push_back(panel, U_SIDE);
  // 整理位置
  v_main->locate();
}
void Program::handle() {
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
void Program::print() {
  // print the result and you can output to a file.
}
void Program::trigger(int idx) {
  std::vector<bool> old = function;
  switch (idx) {
  case 0: {
    if (!function[0]) {
      function[1] = false;
      function[3] = false;
    }
    function[0].flip();
    break;
  }
  case 1: {
    if (!function[1]) {
      function[0] = false;
      function[3] = false;
    }
    function[1].flip();
    break;
  }
  case 2: {
    function[2].flip();
    break;
  }
  case 3: {
    if (!function[3]) {
      function[1] = false;
      function[0] = false;
    }
    function[3].flip();
    break;
  }
  default:;
  }
  for (int i = 0; i < 4; i++) {
    if ((old[i] == false && function[i] == true) ||
        (old[i] == true && function[i] == false))
      set_select_flag[i]();
  }
}
void Program::draw() {
  SDL_SetRenderTarget(render, view);
  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
  SDL_RenderClear(render);
  v_main->draw(render, event);
  SDL_SetRenderTarget(render, nullptr);
  SDL_RenderTexture(render, view, nullptr, nullptr);
  SDL_RenderPresent(render);
}
bool Program::is_quit(SDL_Event event) {
  return (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_Q &&
          event.key.mod & SDL_KMOD_CTRL);
}
void Program::init() {
  window =
      SDL_CreateWindow("Program", scr_size.x, scr_size.y, SDL_WINDOW_RESIZABLE);
  render = SDL_CreateRenderer(window, nullptr);
}
void Program::set_view() {
  if (view != nullptr) {
    SDL_DestroyTexture(view);
  }
  view = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
                           SDL_TEXTUREACCESS_TARGET, scr_size.x, scr_size.y);
}
void Program::run() {
  do {
    handle();
    draw();
  } while (!is_quit(event) && !request_quit);
}