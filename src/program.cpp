#include "program.h"
void Program::create_map() {
  auto map = std::make_shared<Map>(map_size, 18);
  set_key = std::bind(&Map::set_key, map, code);
  set_function = std::bind(&Map::set_function, map, function);
  v_main->push_back(map, CENTER);
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
    if (event.key.mod & SDL_KMOD_SHIFT) {
      if (event.key.key >= (int)'a' &&
          event.key.key <= (int)'z') {
        code = event.key.key - 'a' + 'A';
      } else {
        switch (event.key.key) {
        case '`':
          code = '~';
          break;
        case '1':
          code = '!';
          break;
        case '2':
          code = '@';
          break;
        case '3':
          code = '#';
          break;
        case '4':
          code = '$';
          break;
        case '5':
          code = '%';
          break;
        case '6':
          code = '^';
          break;
        case '7':
          code = '&';
          break;
        case '8':
          code = '*';
          break;
        case '9':
          code = '(';
          break;
        case '0':
          code = ')';
          break;
        case '-':
          code = '_';
          break;
        case '=':
          code = '+';
          break;
        case '[':
          code = '{';
          break;
        case ']':
          code = '}';
          break;
        case '\\':
          code = '|';
          break;
        case ';':
          code = ':';
          break;
        case '\'':
          code = '\"';
          break;
        case ',':
          code = '<';
          break;
        case '.':
          code = '>';
          break;
        case '/':
          code = '?';
          break;
        }
      }
    } else if (event.key.mod & SDL_KMOD_CAPS) {
      code = event.key.key - 'a' + 'A';
    } else {
      code = event.key.key;
    }
    set_key(code);
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
  set_function(function);
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