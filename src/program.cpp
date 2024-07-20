#include "program.h"
#include <format>
#include <iostream>
#define FPS_LIMIT 200

void Program::create_map() {
  map = std::make_shared<Map>(map_size, tile_size, render);
  map->font = font.get_font("Terminus.ttf", 16);
  map->fcolor = {0, 0, 0, 255};
  label_key = std::make_shared<Label>();
  label_key->font = font.get_font("Terminus.ttf", 16);
  label_key->fcolor = {0, 0, 0, 255};
  label_key->set_text(" ", render);
  v_main->push_back(label_key, LU_CORNER, 1, 1);
  label_position = std::make_shared<Label>();
  label_position->font = font.get_font("Terminus.ttf", 16);
  label_position->fcolor = {0, 0, 0, 255};
  label_position->set_text("<025,520>", render);
  v_main->push_back(label_position, RU_CORNER, 2, 2);
  v_main->push_back(map, CENTER, 3, 0);
}

void Program::create_v_main() {
  v_main = std::make_shared<View>(scr_size);
  create_map();
  SDL_Texture *texture = graphic.get_tile("tool", "select");
  SDL_Texture *texture_point = graphic.get_tile("tool", "point");
  SDL_Texture *texture_rect = graphic.get_tile("tool", "rect");
  SDL_Texture *texture_eraser = graphic.get_tile("tool", "eraser");
  SDL_Texture *texture_bucket = graphic.get_tile("tool", "bucket");
  SDL_Texture *texture_free = graphic.get_tile("tool", "free");
  SDL_Texture *texture_circle = graphic.get_tile("tool", "circle");
  SDL_Texture *texture_box = graphic.get_tile("tool", "box");
  SDL_Texture *texture_line = graphic.get_tile("tool", "line");
  SDL_Texture *texture_saw = graphic.get_tile("tool", "saw");
  SDL_Texture *texture_choose = graphic.get_tile("tool", "choose");
  SDL_Texture *texture_move = graphic.get_tile("tool", "move");
  auto point = std::make_shared<Check>(texture_point, texture);
  auto rect = std::make_shared<Check>(texture_rect, texture);
  auto eraser = std::make_shared<Check>(texture_eraser, texture);
  auto bucket = std::make_shared<Check>(texture_bucket, texture);
  auto free_paint = std::make_shared<Check>(texture_free, texture);
  auto circle = std::make_shared<Check>(texture_circle, texture);
  auto box = std::make_shared<Check>(texture_box, texture);
  auto line = std::make_shared<Check>(texture_line, texture);
  auto saw = std::make_shared<Check>(texture_saw, texture);
  auto choose = std::make_shared<Check>(texture_choose, texture);
  auto move = std::make_shared<Check>(texture_move, texture);
  constexpr glm::fvec2 enlarge_tile(32, 32);
  point->resize(enlarge_tile);
  rect->resize(enlarge_tile);
  eraser->resize(enlarge_tile);
  bucket->resize(enlarge_tile);
  free_paint->resize(enlarge_tile);
  circle->resize(enlarge_tile);
  box->resize(enlarge_tile);
  line->resize(enlarge_tile);
  saw->resize(enlarge_tile);
  choose->resize(enlarge_tile);
  move->resize(enlarge_tile);
  point->press = [this] { trigger(1); };
  rect->press = [this] { trigger(2); };
  eraser->press = [this] { trigger(-1); };
  bucket->press = [this] { trigger(3); };
  free_paint->press = [this] { trigger(4); };
  circle->press = [this] { trigger(5); };
  box->press = [this] { trigger(6); };
  line->press = [this] { trigger(7); };
  saw->press = [this] { trigger(8); };
  choose->press = [this] { trigger(-2); };
  move->press = [this] { trigger(-3); };
  set_select_flag.emplace_back([point] { point->activate(); });           // 1
  set_select_flag.emplace_back([rect] { rect->activate(); });             // 2
  set_select_flag.emplace_back([bucket] { bucket->activate(); });         // 3
  set_select_flag.emplace_back([free_paint] { free_paint->activate(); }); // 4
  set_select_flag.emplace_back([circle] { circle->activate(); });         // 5
  set_select_flag.emplace_back([box] { box->activate(); });               // 6
  set_select_flag.emplace_back([line] { line->activate(); });             // 7
  set_select_flag.emplace_back([saw] { saw->activate(); });               // 8
  set_select_flag.emplace_back([move] { move->activate(); });             //-3
  set_select_flag.emplace_back([choose] { choose->activate(); });         //-2
  set_select_flag.emplace_back([eraser] { eraser->activate(); });         //-1
  const auto panel = std::make_shared<Box>(false);
  panel->push_back(point);      // click once
  panel->push_back(rect);       // twice
  panel->push_back(bucket);     // once
  panel->push_back(free_paint); // always
  panel->push_back(circle);     // twice
  panel->push_back(box);        // twice
  panel->push_back(line);       // twice
  panel->push_back(saw);        // always
  panel->push_back(move);
  panel->push_back(choose);
  panel->push_back(eraser);
  // 整理大小
  panel->set_size();
  const auto save =
      std::make_shared<Check>(graphic.get_tile("tool", "save"), nullptr);
  save->resize(enlarge_tile * 2.0f);
  save->press = [this] { print(); };
  v_main->push_back(save, D_SIDE, -1, 4);
  v_main->push_back(panel, U_SIDE, 0, 3);
  // 整理位置
  v_main->locate();
}

void Program::handle() {
  SDL_PollEvent(&event);
  if (event.type == SDL_EVENT_KEY_DOWN) {
    if (event.key.mod & SDL_KMOD_SHIFT) {
      if (event.key.key >= static_cast<int>('a') &&
          event.key.key <= static_cast<int>('z')) {
        keycode = static_cast<char>(event.key.key - 'a' + 'A');
      } else {
        switch (event.key.key) {
        case '`':
          keycode = '~';
          break;
        case '1':
          keycode = '!';
          break;
        case '2':
          keycode = '@';
          break;
        case '3':
          keycode = '#';
          break;
        case '4':
          keycode = '$';
          break;
        case '5':
          keycode = '%';
          break;
        case '6':
          keycode = '^';
          break;
        case '7':
          keycode = '&';
          break;
        case '8':
          keycode = '*';
          break;
        case '9':
          keycode = '(';
          break;
        case '0':
          keycode = ')';
          break;
        case '-':
          keycode = '_';
          break;
        case '=':
          keycode = '+';
          break;
        case '[':
          keycode = '{';
          break;
        case ']':
          keycode = '}';
          break;
        case '\\':
          keycode = '|';
          break;
        case ';':
          keycode = ':';
          break;
        case '\'':
          keycode = '\"';
          break;
        case ',':
          keycode = '<';
          break;
        case '.':
          keycode = '>';
          break;
        case '/':
          keycode = '?';
          break;
        default:;
        }
      }
    } else if (event.key.mod & SDL_KMOD_CAPS) {
      keycode = static_cast<char>(event.key.key - 'a' + 'A');
    } else {
      keycode = static_cast<char>(event.key.key);
    }
    map->set_key(keycode);
    char tmp[2];
    tmp[0] = keycode;
    tmp[1] = '\0';
    label_key->set_text(tmp, render);
  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    v_main->pressed(event);
  } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
    // windows resize
    scr_size.x = event.window.data1;
    scr_size.y = event.window.data2;
    set_view();
    v_main->set_screen_size(scr_size);
    v_main->locate();
  } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
    request_quit = true;
  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
    v_main->released(event);
  } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
    v_main->hovering(event);
  }
  if (map->in()) {
    glm::ivec2 mouse_position = map->get_grid();
    const std::string str_position =
        std::format("<{},{}>", mouse_position.x, mouse_position.y);
    label_position->set_text(str_position, render);
  }
  map->set_function(select_type, special_action);
}

void Program::print() const {
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
  // TODO
  // 填充功能与set不是很配合，因为set只是将相同的放到了一起，没有判断是否相连，判断相连工作量巨大，需要判断这个点和内部的所有点是否有相连的，如果有就加入，否则就另外建一个set,这个样子导致我们的原始数据结构也不行。emmm,不过考虑到判断相连不会很麻烦，所以我觉得最难的在与优化，举个例子，如果我能得到一个多边形，然后判断点与多边形的关系。
  // 上面的问题解决了，通过在data里面判断周围是否存在某个集合就行，如果存在就加入这个集合
  // TODO 蓝图功能引入非常关键，之前的很多就会缺少直观的选取
  // TODO 画饼
  // TODO 画框
  // TODO 画线
  // TODO
  // 画锯齿，也就是类似海岸线的状态，你可以让他在绘制的一定范围内波动，但是要求连接。有点像正弦函数，举个例子，你绘制了从一个点到另一个点会绘制经过这两个点的正弦曲线。
  // TODO
  // 选择状态，相当于擦除的对立功能，选择后会停留在选取，使用移动可以移动选取,这个时候会取消选取功能进入移动功能，而与擦除也是对立的，不能同时生效。而直接移动可以与其他的配合，但是只能移动一次选中的内容。首先点击选中，然后左键长按会进入移动状态，放下停止该次移动。而可以重复移动。更换功能会清除选取内容。
  // TODO 移动选中的值
  // TODO 随机化填充功能，需要设定一个填充比例
  // TODO 上下层功能
  // int old_select_type = select_type;
  // int old_special_action = special_action;
  // if (idx > 0) {
  //   select_type = idx;
  //   special_action = 0;
  // } else if (idx < 0) {
  //   special_action = idx;
  //   select_type = 0;
  // } else {
  //   select_type = 0;
  //   special_action = 0;
  // }
  // FIXME
  // 由于上面的意思是如果选了左侧功能，右侧功能区会自动被激活，这是一个问题，按理说两侧功能区不影响
  // old是当前正处于激活状态的
  // 否则就是当前的选择的功能
  // 如果当前的一致就只执行这个
  // 如果不同就上一个与该功能同时执行激活
  // 如果当前激活的是不同侧的功能，那么当前侧的不影响，之激活另一侧的，另一侧的待机是0
  size_t length = set_select_flag.size();
  if (idx > 0) {
    // 左侧功能
    if (idx != select_type && select_type != 0) {
      // cancel old function is different from current select.
      set_select_flag[select_type - 1]();
    }
    // enable function
    set_select_flag[idx - 1]();
    if (idx != select_type)
      select_type = idx;
    else
      select_type = 0;
  } else if (idx < 0) {
    // 右侧功能
    if (idx != special_action && special_action != 0) {
      set_select_flag[length + special_action]();
    }
    set_select_flag[length + idx]();
    if (idx != special_action)
      special_action = idx;
    else
      special_action = 0;
  } else {
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
  graphic.set_render(render);
}

void Program::set_view() {
  if (view != nullptr) {
    SDL_DestroyTexture(view);
  }
  view = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
                           SDL_TEXTUREACCESS_TARGET, scr_size.x, scr_size.y);
}

void Program::run() {
  const int DELAY_TIME = 1000 / FPS_LIMIT;
  // Delay time in milliseconds for each frame

  Uint32 frameStart;
  int frameTime;
  do {
    frameStart = SDL_GetTicks();
    handle();
    draw();
    frameTime = SDL_GetTicks() - frameStart;

    if (frameTime < DELAY_TIME) {
      SDL_Delay(DELAY_TIME - frameTime);
    }
  } while (!is_quit(event) && !request_quit);
}
