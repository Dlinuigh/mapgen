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
  // FIXME texture每次都被清空了
  bg = TTF_RenderUTF8_Solid(font, &value, fcolor);
  float w = bg->w;
  float h = bg->h;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_FRect dst = {area.x + (area.w - w) / 2, area.y + (area.h - h) / 2, w, h};
  SDL_RenderTexture(render, texture, nullptr, &dst);
  SDL_DestroyTexture(texture);
}
void Map::draw_tile(SDL_Renderer *render, glm::ivec2 pos) {
  char draw_char = function_state[2] ? ' ' : code;
  SDL_FRect dst = get_area(pos);
  SDL_Surface *surface = graphic.get_char(draw_char, font, fcolor);
  float w = surface->w;
  float h = surface->h;
  SDL_FRect draw_text_area = {dst.x + (dst.w - w) / 2, dst.y + (dst.h - h) / 2,
                              w, h};
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
  SDL_RenderTexture(render, texture, nullptr, &draw_text_area);
  SDL_DestroyTexture(texture);
}
void Map::draw(SDL_Renderer *render, SDL_Event) {
  // first won't clear texture, just replace.
  SDL_Texture* target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, map_view);
  if (function_state[0]) {
    draw_tile(render, start_pos);
  } else if (function_state[1]) {
    int min_x = std::min(start_pos.x, end_pos.x);
    int min_y = std::min(start_pos.y, end_pos.y);
    int max_x = std::max(start_pos.x, end_pos.x);
    int max_y = std::max(start_pos.y, end_pos.y);
    for (int i = min_x; i < max_x; i++) {
      for (int j = min_y; j < max_y; j++) {
        draw_tile(render, glm::ivec2(i, j));
      }
    }
  } else if (function_state[3]) {
    char target = data[start_pos.x][start_pos.y];
    std::vector<glm::ivec2> adj_set;
    dfs(start_pos.x, start_pos.y, adj_set, target);
    for (auto &it : adj_set) {
      draw_tile(render, it);
    }
  }
  SDL_SetRenderTarget(render, target);
  SDL_RenderTexture(render, map_view, nullptr, &area);
}
bool Map::click() {
  if (in()) {
    if (!function_state[1])
      start_pos = get_grid();
    else {
      // Rect function
      end_pos = get_grid();
    }
    printf("%d, %d\n", start_pos.x, start_pos.y);
    return true;
  } else {
    return false;
  }
}
void Map::set_key(char c) {
  code = c;
  printf("the key is '%c'\n", c);
}
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
  // FIXME 似乎有问题
  SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
  return glm::ivec2((mouse_pos.x - area.x) / tile_size,
                    (mouse_pos.y - area.y) / tile_size);
}
SDL_FRect Map::get_area(glm::ivec2 pos) {
  return {area.x + pos.x * tile_size, area.y + pos.y * tile_size, tile_size,
          tile_size};
}