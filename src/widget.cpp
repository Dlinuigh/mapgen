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
}
void Widget::resize(float length, bool horizon) {
  if (horizon) {
    area.w = length;
    area.h = length / ratio;
  } else {
    area.h = length;
    area.w = length * ratio;
  }
}
void Widget::locate(glm::fvec2 position) {
  area.x = position.x;
  area.y = position.y;
}
void Widget::set_desire_size(glm::fvec2 size) {
  area.w = size.x;
  area.h = size.y;
}
void Check::set_check_texture(SDL_Surface *surface) { check_sign = surface; }
void Check::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_RenderTexture(render, texture, nullptr, &area);
  SDL_DestroyTexture(texture);
  if (activated) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, check_sign);
    SDL_RenderTexture(render, texture, nullptr, &area);
    SDL_DestroyTexture(texture);
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
void Cell::set_value(char c) {
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
  bg = TTF_RenderUTF8_Solid(font, &value, fcolor);
  float w = bg->w;
  float h = bg->h;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_FRect dst = {area.x + (area.w - w) / 2, area.y + (area.h - h) / 2, w, h};
  SDL_RenderTexture(render, texture, nullptr, &dst);
  SDL_DestroyTexture(texture);
}
void Map::draw_char(SDL_Renderer *render, SDL_FRect dst) {
  SDL_Surface *surface = graphic.get_char(code, font, fcolor);
  float w = surface->w;
  float h = surface->h;
  SDL_FRect draw_text_area = {dst.x + (dst.w - w) / 2, dst.y + (dst.h - h) / 2,
                              w, h};
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
  SDL_RenderTexture(render, texture, nullptr, &draw_text_area);
  SDL_DestroyTexture(texture);
}
void Map::clear_tile(SDL_Renderer *render, SDL_FRect dst) {
  SDL_SetRenderDrawColor(render, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
  SDL_RenderFillRect(render, &dst);
}
void Map::draw_tile(SDL_Renderer *render, glm::ivec2 pos) {
  SDL_FRect dst = get_area(pos);
  if (function_state[2]) {
    // clear
    clear_tile(render, dst);
  } else {
    // 对于rect可能出现，覆盖相同的tile,重绘，但是我不管。s
    clear_tile(render, dst);
    draw_char(render, dst);
  }
}
void Map::draw_grid(SDL_Renderer *render) {
  for (int i = 0; i <= size.x; i++) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    if (i % 5 == 0 && i != 0 && i != size.x) {
      SDL_SetRenderDrawColor(render, 0, 0, 255, SDL_ALPHA_OPAQUE);
    }
    if (i % 10 == 0 && i != 0 && i != size.x) {
      SDL_SetRenderDrawColor(render, 255, 0, 255, SDL_ALPHA_OPAQUE);
    }
    SDL_FRect line = {tile_size * (i + 1), 0, 1, tile_size * size.y};
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
    SDL_FRect line = {0, tile_size * (j + 1), tile_size * size.x, 1};
    SDL_RenderFillRect(render, &line);
  }
}
void Map::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, map_view);
  if (begin_draw) {
    if (function_state[0]) {
      // 对于dfs会提前修改data
      data[start_pos.x][start_pos.y] = function_state[2] ? ' ' : code;
      draw_tile(render, start_pos);
    } else if (function_state[1]) {
      int min_x = std::min(start_pos.x, end_pos.x);
      int min_y = std::min(start_pos.y, end_pos.y);
      int max_x = std::max(start_pos.x, end_pos.x);
      int max_y = std::max(start_pos.y, end_pos.y);
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          data[i][j] = function_state[2] ? ' ' : code;
          draw_tile(render, glm::ivec2(i, j));
        }
      }
    } else if (function_state[3]) {
      char find_this_same = data[start_pos.x][start_pos.y];
      std::vector<glm::ivec2> adj_set;
      sign.assign(size.x, std::vector<bool>(size.y, false));
      dfs(start_pos.x, start_pos.y, adj_set, find_this_same);
      for (auto &it : adj_set) {
        draw_tile(render, it);
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
void Map::set_key(char c) { code = c; }
void Map::set_function(std::vector<bool> func) { function_state = func; }
bool Map::is_valid(int x, int y) {
  return x >= 0 && y >= 0 && x < size.x && y < size.y;
}
void Map::dfs(int x, int y, std::vector<glm::ivec2> &adj_set, char target) {
  if (!is_valid(x, y)) {
    return;
  } else if ((sign[x][y] == true)) {
    return;
  } else if (data[x][y] != target) { // 找到所有target的连接块
    return;
  }

  // 将当前点加入相邻点集合
  adj_set.push_back(glm::ivec2(x, y));

  // 标记当前点已访问
  data[x][y] = function_state[2] ? ' ' : code; // 清空或者赋值
  sign[x][y] = true;

  // 定义四个方向的偏移量
  int dx[] = {-1, 1, 0, 0};
  int dy[] = {0, 0, -1, 1};

  // 对四个方向进行搜索
  for (int i = 0; i < 4; ++i) {
    int nx = x + dx[i];
    int ny = y + dy[i];
    dfs(nx, ny, adj_set, target);
  }
}
glm::ivec2 Map::get_grid() {
  glm::fvec2 mouse_pos;
  SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
  return glm::ivec2((mouse_pos.x - area.x) / (int)tile_size,
                    (mouse_pos.y - area.y) / (int)tile_size);
}
SDL_FRect Map::get_area(glm::ivec2 pos) {
  return {pos.x * tile_size, pos.y * tile_size, tile_size, tile_size};
}
void Label::set_text(std::string _text) {
  text = std::move(_text);
  surface = TTF_RenderUTF8_Solid(font, text.c_str(), fcolor);
  area.w = surface->w;
  area.h = surface->h;
}
void Label::draw(SDL_Renderer *render, SDL_Event) {
  if(bg!=nullptr){
    SDL_Texture* texture = SDL_CreateTextureFromSurface(render, bg);
    SDL_RenderTexture(render, texture, nullptr, &area);
    SDL_DestroyTexture(texture);
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
  SDL_RenderTexture(render, texture, nullptr, &area);
  SDL_DestroyTexture(texture);
}