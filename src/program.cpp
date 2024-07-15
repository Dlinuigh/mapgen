#include "program.h"
#include <format>
#include <iostream>

void Program::create_map() {
  map = std::make_shared<Map>(map_size, tile_size, render);
  map->font = font.get_font("Terminus.ttf", 16);
  map->fcolor = {0, 0, 0, 255};
  label_key = std::make_shared<Label>();
  label_key->font = font.get_font("Terminus.ttf", 16);
  label_key->fcolor = {0, 0, 0, 255};
  label_key->set_text(" ");
  v_main->push_back(label_key, LU_CORNER);
  label_position = std::make_shared<Label>();
  label_position->font = font.get_font("Terminus.ttf", 16);
  label_position->fcolor = {0, 0, 0, 255};
  label_position->set_text("<025,520>");
  v_main->push_back(label_position, RU_CORNER);
  v_main->push_back(map, CENTER);
}

void Program::create_v_main() {
  v_main = std::make_shared<View>(scr_size);
  create_map();
  auto point = std::make_shared<Check>(graphic.get_tile("tool", "point"));
  auto rect = std::make_shared<Check>(graphic.get_tile("tool", "rect"));
  auto eraser = std::make_shared<Check>(graphic.get_tile("tool", "eraser"));
  auto bucket = std::make_shared<Check>(graphic.get_tile("tool", "bucket"));
  const auto save = std::make_shared<Check>(graphic.get_tile("tool", "save"));
  // FIXME 下面的四个surface都发生了leak
  point->set_check_texture(graphic.get_tile("tool", "select"));
  rect->set_check_texture(graphic.get_tile("tool", "select"));
  eraser->set_check_texture(graphic.get_tile("tool", "select"));
  bucket->set_check_texture(graphic.get_tile("tool", "select"));
   constexpr glm::fvec2 enlarge_tile(64, 64);
  point->set_desire_size(enlarge_tile);
  rect->set_desire_size(enlarge_tile);
  eraser->set_desire_size(enlarge_tile);
  bucket->set_desire_size(enlarge_tile);
  save->set_desire_size(enlarge_tile);
  point->callback = [this] { trigger(0); };
  rect->callback = [this] { trigger(1); };
  eraser->callback = [this] { trigger(2); };
  bucket->callback = [this] { trigger(3); };
  save->callback = [this] { print(); };
  set_select_flag.emplace_back([point] { point->activate(); });
  set_select_flag.emplace_back([rect] { rect->activate(); });
  set_select_flag.emplace_back([eraser] { eraser->activate(); });
  set_select_flag.emplace_back([bucket] { bucket->activate(); });
  const auto panel = std::make_shared<Box>(false);
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
      if (event.key.key >= static_cast<int>('a') &&
          event.key.key <= static_cast<int>('z')) {
        code = static_cast<char>(event.key.key - 'a' + 'A');
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
        default:;
        }
      }
    } else if (event.key.mod & SDL_KMOD_CAPS) {
      code = static_cast<char>(event.key.key - 'a' + 'A');
    } else {
      code = static_cast<char>(event.key.key);
    }
    map->set_key(code);
    char tmp[2];
    tmp[0] = code;
    tmp[1] = '\0';
    label_key->set_text(tmp);
  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
             event.button.button == SDL_BUTTON_LEFT) {
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
  } else if (event.type == SDL_EVENT_MOUSE_MOTION && map->in() &&
             !right_button_down) {
    glm::ivec2 mouse_position = map->get_grid();
    const std::string str_position =
        std::format("<{},{}>", mouse_position.x, mouse_position.y);
    label_position->set_text(str_position);
  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
             event.button.button == SDL_BUTTON_RIGHT) {
    right_button_down = true;
    SDL_GetMouseState(&right_button_position.x, &right_button_position.y);
    map_old_position.x = map->area.x;
    map_old_position.y = map->area.y;
  } else if (event.type == SDL_EVENT_MOUSE_MOTION && right_button_down) {
    glm::fvec2 tmp_position;
    SDL_GetMouseState(&tmp_position.x, &tmp_position.y);
    map->area.x =
        map_old_position.x + (tmp_position.x - right_button_position.x);
    map->area.y =
        map_old_position.y + (tmp_position.y - right_button_position.y);
  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
             event.button.button == SDL_BUTTON_RIGHT) {
    right_button_down = false;
  }
}

void Program::print() const {
  // print the result and you can output to a file.
  Json::Value document;
  Json::Value m(Json::arrayValue);
  for (int j = 0; j < map_size.x; j++) {
    std::string line;
    for (int i = 0; i < map_size.y; i++) {
      line.push_back(map->data[i][j]);
    }
    m.append(line);
  }
  document["main"] = m;
  const Json::StreamWriterBuilder builder;
  const std::string output = writeString(builder, document);
  std::cout << output << std::endl;
}

void Program::trigger(const int idx) {
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
  map->set_function(function);
  for (int i = 0; i < 4; i++) {
    if ((old[i] == false && function[i] == true) ||
        (old[i] == true && function[i] == false))
      set_select_flag[i]();
  }
}

void Program::draw() const {
  SDL_SetRenderTarget(render, view);
  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
  SDL_RenderClear(render);
  v_main->draw(render, event);
  SDL_SetRenderTarget(render, nullptr);
  SDL_RenderTexture(render, view, nullptr, nullptr);
  SDL_RenderPresent(render);
}

bool Program::is_quit(const SDL_Event &event) {
  return event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_Q &&
          event.key.mod & SDL_KMOD_CTRL;
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
