#include "widget.h"

bool Widget::in() const {
  glm::fvec2 mouse;
  SDL_GetMouseState(&mouse.x, &mouse.y);
  return mouse.x > area.x && mouse.x <= area.x + area.w && mouse.y > area.y &&
         mouse.y <= area.y + area.h;
}

bool Widget::click() {
  if (in() && callback != nullptr) {
    callback();
    return true;
  }
  return false;
}

void Widget::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_RenderTexture(render, texture, nullptr, &area);
  SDL_DestroyTexture(texture);
}

void Widget::resize(const float length, const bool horizon) {
  if (horizon) {
    area.w = length;
    area.h = length / ratio;
  } else {
    area.h = length;
    area.w = length * ratio;
  }
}

void Widget::locate(const glm::fvec2 position) {
  area.x = position.x;
  area.y = position.y;
}

void Widget::set_desire_size(const glm::fvec2 size) {
  area.w = size.x;
  area.h = size.y;
}

void Check::set_check_texture(SDL_Surface *surface) { check_sign = surface; }

void Check::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_RenderTexture(render, texture, nullptr, &area);
  SDL_DestroyTexture(texture);
  if (activated) {
    SDL_Texture *sdl_texture = SDL_CreateTextureFromSurface(render, check_sign);
    SDL_RenderTexture(render, sdl_texture, nullptr, &area);
    SDL_DestroyTexture(sdl_texture);
  }
}

bool Check::click() {
  if (in() && callback != nullptr) {
    callback();
    return true;
  } else {
    return false;
  }
}

void Check::activate() { activated = !activated; }

void Cell::set_value(const char c) {
  // for external function callback to change value to a variable value.
  value = c;
}

bool Cell::click() {
  if (in() && callback != nullptr) {
    callback();
    return true;
  } else {
    return false;
  }
}

void Cell::draw(SDL_Renderer *render, SDL_Event) {
  if (bg != nullptr) {
    SDL_DestroySurface(bg);
  }
  bg = TTF_RenderUTF8_Solid(font, &value, fcolor);
  const auto w = static_cast<float>(bg->w);
  const auto h = static_cast<float>(bg->h);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  const SDL_FRect dst = {area.x + (area.w - w) / 2, area.y + (area.h - h) / 2,
                         w, h};
  SDL_RenderTexture(render, texture, nullptr, &dst);
  SDL_DestroyTexture(texture);
}

void Map::draw_char(SDL_Renderer *render, const SDL_FRect dst) const {
  SDL_Surface *surface = graphic.get_char(code, font, fcolor);
  const auto w = static_cast<float>(surface->w);
  const auto h = static_cast<float>(surface->h);
  const SDL_FRect draw_text_area = {dst.x + (dst.w - w) / 2,
                                    dst.y + (dst.h - h) / 2, w, h};
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
  SDL_RenderTexture(render, texture, nullptr, &draw_text_area);
  SDL_DestroyTexture(texture);
}

void Map::clear_tile(SDL_Renderer *render, const SDL_FRect dst) const {
  SDL_SetRenderDrawColor(render, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
  SDL_RenderFillRect(render, &dst);
}

void Map::draw_tile(SDL_Renderer *render, const glm::ivec2 pos) {
  const SDL_FRect dst = get_area(pos);
  if (function_state[2]) {
    clear_tile(render, dst);
  } else {
    clear_tile(render, dst);
    draw_char(render, dst);
  }
}

void Map::draw_grid(SDL_Renderer *render) const {
  for (int i = 0; i <= size.x; i++) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    if (i % 5 == 0 && i != 0 && i != size.x) {
      SDL_SetRenderDrawColor(render, 0, 0, 255, SDL_ALPHA_OPAQUE);
    }
    if (i % 10 == 0 && i != 0 && i != size.x) {
      SDL_SetRenderDrawColor(render, 255, 0, 255, SDL_ALPHA_OPAQUE);
    }
    SDL_FRect line = {tile_size * static_cast<float>(i + 1), 0, 1,
                      tile_size * static_cast<float>(size.y)};
    SDL_RenderFillRect(render, &line);
  }
  for (int j = 0; j <= size.y; j++) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    if (j % 5 == 0 && j != 0 && j != size.y) {
      SDL_SetRenderDrawColor(render, 0, 0, 255, SDL_ALPHA_OPAQUE);
    }
    if (j % 10 == 0 && j != 0 && j != size.y) {
      SDL_SetRenderDrawColor(render, 255, 0, 255, SDL_ALPHA_OPAQUE);
    }
    SDL_FRect line = {0, tile_size * static_cast<float>(j + 1),
                      tile_size * static_cast<float>(size.x), 1};
    SDL_RenderFillRect(render, &line);
  }
}

void Map::draw(SDL_Renderer *render, SDL_Event event) {
  SDL_Texture *target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, map_view);
  if (begin_draw) {
    if (function_state[0]) {
      // 对于dfs会提前修改data
      data[start_pos.x][start_pos.y] = function_state[2] ? ' ' : code;
      draw_tile(render, start_pos);
    } else if (function_state[1]) {
      const int min_x = std::min(start_pos.x, end_pos.x);
      const int min_y = std::min(start_pos.y, end_pos.y);
      const int max_x = std::max(start_pos.x, end_pos.x);
      const int max_y = std::max(start_pos.y, end_pos.y);
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          data[i][j] = function_state[2] ? ' ' : code;
          draw_tile(render, glm::ivec2(i, j));
        }
      }
    } else if (function_state[3]) {
      const char find_this_same = data[start_pos.x][start_pos.y];
      std::vector<glm::ivec2> adj_set;
      sign.assign(size.x, std::vector(size.y, false));
      dfs(start_pos.x, start_pos.y, adj_set, find_this_same);
      for (const auto &it : adj_set) {
        draw_tile(render, it);
      }
    } else if (function_state[4]) {
      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        left_button_down = true;
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
                 event.button.button == SDL_BUTTON_RIGHT) {
        left_button_down = false;
      } else if (event.type == SDL_EVENT_MOUSE_MOTION && left_button_down) {
        glm::ivec2 pos = get_grid();
        draw_tile(render, pos);
      }
    }
    begin_draw = false;
  }
  draw_grid(render);
  SDL_SetRenderTarget(render, target);
  SDL_RenderTexture(render, map_view, nullptr, &area);
}

bool Map::click() {
  if (in()) {
    if (!function_state[1]) {
      start_pos = get_grid();
      begin_draw = true;
    } else {
      // Rect function
      if (already_select_one) {
        end_pos = get_grid();
        begin_draw = true;
        already_select_one = false;
      } else {
        start_pos = get_grid();
        already_select_one = true;
      }
    }
    return true;
  } else {
    return false;
  }
}

void Map::set_key(const char c) { code = c; }
void Map::set_function(const std::vector<bool> &func) { function_state = func; }

bool Map::is_valid(const int x, const int y) const {
  return x >= 0 && y >= 0 && x < size.x && y < size.y;
}

void Map::dfs(const int startX, const int startY,
              std::vector<glm::ivec2> &adj_set, const char target) {
  // 定义四个方向的偏移量
  constexpr int dx[] = {-1, 1, 0, 0};
  constexpr int dy[] = {0, 0, -1, 1};

  // 创建一个栈用于存储要访问的节点
  std::stack<std::pair<int, int>> stack;
  stack.emplace(startX, startY);

  while (!stack.empty()) {
    auto [x, y] = stack.top();
    stack.pop();

    // 检查当前点是否有效，是否已访问过，是否与目标字符匹配
    if (!is_valid(x, y) || sign[x][y] || data[x][y] != target) {
      continue;
    }

    // 将当前点加入相邻点集合
    adj_set.emplace_back(x, y);

    // 标记当前点已访问
    data[x][y] = function_state[2] ? ' ' : code; // 清空或者赋值
    sign[x][y] = true;

    // 将四个方向的相邻点压入栈中
    for (int i = 0; i < 4; ++i) {
      const int nx = x + dx[i];
      const int ny = y + dy[i];
      stack.emplace(nx, ny);
    }
  }
}

glm::ivec2 Map::get_grid() const {
  glm::fvec2 mouse_pos;
  SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
  return {static_cast<int>((mouse_pos.x - area.x) / tile_size),
          static_cast<int>((mouse_pos.y - area.y) / tile_size)};
}

SDL_FRect Map::get_area(const glm::ivec2 pos) const {
  return {static_cast<float>(pos.x) * tile_size,
          static_cast<float>(pos.y) * tile_size, tile_size, tile_size};
}

void Label::set_text(std::string _text) {
  text = std::move(_text);
  if (surface != nullptr) {
    SDL_DestroySurface(surface);
  }
  surface = TTF_RenderUTF8_Solid(font, text.c_str(), fcolor);
  area.w = static_cast<float>(surface->w);
  area.h = static_cast<float>(surface->h);
}

void Label::draw(SDL_Renderer *render, SDL_Event) {
  if (bg != nullptr) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
    SDL_RenderTexture(render, texture, nullptr, &area);
    SDL_DestroyTexture(texture);
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
  SDL_RenderTexture(render, texture, nullptr, &area);
  SDL_DestroyTexture(texture);
}
