#include "widget.h"
bool Widget::in() const {
  glm::fvec2 mouse;
  SDL_GetMouseState(&mouse.x, &mouse.y);
  return mouse.x > area.x && mouse.x <= area.x + area.w && mouse.y > area.y &&
         mouse.y <= area.y + area.h;
}
void Widget::draw(SDL_Renderer *, SDL_Event) {}
bool Widget::pressed(SDL_Event &) { return false; }
bool Widget::released(SDL_Event &) { return false; }
bool Widget::hovering(SDL_Event &) { return false; }
void Widget::locate(const glm::fvec2 position) {
  area.x = position.x;
  area.y = position.y;
}
void Widget::resize(const glm::fvec2 size) {
  area.w = size.x;
  area.h = size.y;
}
void Check::draw(SDL_Renderer *render, SDL_Event) {
  SDL_RenderTexture(render, bg, nullptr, &area);
  if (activated) {
    SDL_RenderTexture(render, selected, nullptr, &area);
  }
}
bool Check::pressed(SDL_Event &) {
  if (in() && press != nullptr) {
    press();
    return true;
  } else {
    return false;
  }
}
void Check::activate() { activated = !activated; }
void Map::draw_char(SDL_Renderer *render, const SDL_FRect dst) const {
  SDL_Texture *texture_char = graphic.get_char(keycode, font, fcolor);
  float w = 0.0f, h = 0.0f;
  SDL_GetTextureSize(texture_char, &w, &h);
  const SDL_FRect draw_text_area = {dst.x + (dst.w - w) / 2,
                                    dst.y + (dst.h - h) / 2, w, h};
  SDL_RenderTexture(render, texture_char, nullptr, &draw_text_area);
}
void Map::clear_tile(SDL_Renderer *render, const SDL_FRect dst) const {
  SDL_SetRenderDrawColor(render, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
  SDL_RenderFillRect(render, &dst);
}
void Map::draw_tile(SDL_Renderer *render, const glm::ivec2 pos) {
  const SDL_FRect dst = get_area(pos);
  if (special_action == -1) {
    clear_tile(render, dst);
  } else {
    clear_tile(render, dst);
    draw_char(render, dst);
  }
}
void Map::generate_grid(SDL_Renderer *render) const {
  SDL_SetRenderTarget(render, grid);
  for (int i = 0; i <= size.x; i++) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    if (i % 5 == 0 && i != 0 && i != size.x) {
      SDL_SetRenderDrawColor(render, 0, 0, 255, SDL_ALPHA_OPAQUE);
    }
    if (i % 10 == 0 && i != 0 && i != size.x) {
      SDL_SetRenderDrawColor(render, 255, 0, 255, SDL_ALPHA_OPAQUE);
    }
    float x = tile_size * (i + 1);
    SDL_RenderLine(render, x, 0, x, area.h);
    SDL_RenderLine(render, 0, x, area.w, x);
  }
}
void Map::draw_grid(SDL_Renderer *render) const {
  SDL_RenderTexture(render, grid, nullptr, &area);
}
void Map::dfs(SDL_Renderer *render, glm::ivec2 point, char old_code) {
  walked.assign(size.x, std::vector(size.y, false));
  char code = special_action == -1 ? ' ' : keycode;
  std::vector<glm::ivec2> block;
  block.push_back(point);
  data[point.x][point.y] = code;
  walked[point.x][point.y] = true;
  draw_tile(render, point);
  while (!block.empty()) {
    glm::ivec2 pos = block.back();
    block.pop_back();
    std::vector<glm::ivec2> adj = {
        glm::ivec2(pos.x + 1, pos.y),
        glm::ivec2(pos.x - 1, pos.y),
        glm::ivec2(pos.x, pos.y + 1),
        glm::ivec2(pos.x, pos.y - 1),
    };
    for (auto &a : adj) {
      if (is_valid(a.x, a.y) && !walked[a.x][a.y] &&
          data[a.x][a.y] == old_code) {
        block.push_back(a);
        draw_tile(render, a);
        walked[a.x][a.y] = true;
        data[a.x][a.y] = code;
      }
    }
  }
}
void Map::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, bg);
  char code = special_action == -1 ? ' ' : keycode;
  if (begin_draw) {
    switch (select_type) {
    case 1: {
      char old_code = data[pos_start.x][pos_start.y];
      if (old_code != code) {
        data[pos_start.x][pos_start.y] = code;
        draw_tile(render, pos_start);
      }
      begin_draw = false;
      break;
    }
    case 2: {
      const int min_x = std::min(pos_start.x, pos_end.x);
      const int min_y = std::min(pos_start.y, pos_end.y);
      const int max_x = std::max(pos_start.x, pos_end.x);
      const int max_y = std::max(pos_start.y, pos_end.y);
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          if (data[i][j] != code) {
            glm::ivec2 point(i, j);
            data[i][j] = code;
            draw_tile(render, point);
          }
        }
      }
      begin_draw = false;
      break;
    }
    case 3: {
      char old_code = data[pos_start.x][pos_start.y];
      if (old_code != code) {
        dfs(render, pos_start, old_code);
      }
      begin_draw = false;
      break;
    }
    case 4: {
      glm::ivec2 point = get_grid();
      char old_code = data[point.x][point.y];
      if (old_code != code) {
        data[point.x][point.y] = code;
        draw_tile(render, point);
      }
      break;
    }
    case 5: {
      // TODO circle
      const int min_x = std::min(pos_start.x, pos_end.x);
      const int min_y = std::min(pos_start.y, pos_end.y);
      const int max_x = std::max(pos_start.x, pos_end.x);
      const int max_y = std::max(pos_start.y, pos_end.y);
      /*
      椭圆?或者是满足一个尺度,长或者宽,或者经过三个点的圆?我们不是函数绘图软件,我只是一个绘制工具,绘制圆就是越简单的操作越好,我恨不得定一个点,然后鼠标移动模拟圆规
      */
      break;
    }
    case 6: {
      // TODO box empty inside
      const int min_x = std::min(pos_start.x, pos_end.x);
      const int min_y = std::min(pos_start.y, pos_end.y);
      const int max_x = std::max(pos_start.x, pos_end.x);
      const int max_y = std::max(pos_start.y, pos_end.y);
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          if (data[i][j] != code &&
              (i == min_x || i == max_x || j == min_y || j == max_y)) {
            glm::ivec2 point(i, j);
            data[i][j] = code;
            draw_tile(render, point);
          }
        }
      }
      begin_draw = false;
      break;
    }
    case 7: {
      // TODO line
      break;
    }
    case 8:{
      // TODO saw
      break;
    }
    }
  }
  SDL_SetRenderTarget(render, target);
  SDL_RenderTexture(render, bg, nullptr, &area);
  draw_grid(render);
}
bool Map::pressed(SDL_Event &) {
  if (in()) {
    if (select_type == 1 || select_type == 3) {
      pos_start = get_grid();
      begin_draw = true;
    } else if (select_type == 4 || select_type == 8) {
      begin_draw = true;
    } else {
      if (already_selected_one) {
        pos_end = get_grid();
        begin_draw = true;
        already_selected_one = false;
      } else {
        pos_start = get_grid();
        already_selected_one = true;
      }
    }
    return true;
  } else {
    return false;
  }
}
bool Map::hovering(SDL_Event &) {
  if (in())
    return true;
  else
    return false;
}
bool Map::released(SDL_Event &) {
  if (in()) {
    if (select_type == 4) {
      begin_draw = false;
    }
    return true;
  } else {
    return false;
  }
}
void Map::set_key(const char c) { keycode = c; }
void Map::set_function(int _select, int _special) {
  select_type = _select;
  special_action = _special;
}
bool Map::is_valid(const int x, const int y) const {
  return x >= 0 && y >= 0 && x < size.x && y < size.y;
}
glm::ivec2 Map::get_grid() const {
  glm::fvec2 mouse_pos;
  SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
  int x = (mouse_pos.x - area.x) / tile_size;
  int y = (mouse_pos.y - area.y) / tile_size;
  return {std::max(0, std::min(x, size.x - 1)),
          std::max(0, std::min(y, size.y - 1))};
}
SDL_FRect Map::get_area(const glm::ivec2 pos) const {
  float f_tile_size = tile_size;
  return {pos.x * f_tile_size, pos.y * f_tile_size, f_tile_size, f_tile_size};
}
void Label::set_text(std::string _text, SDL_Renderer *render) {
  text = std::move(_text);
  SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text.c_str(), fcolor);
  area.w = static_cast<float>(surface->w);
  area.h = static_cast<float>(surface->h);
  bg = SDL_CreateTextureFromSurface(render, surface);
}
void Label::draw(SDL_Renderer *render, SDL_Event) {
  if (bg != nullptr) {
    SDL_RenderTexture(render, bg, nullptr, &area);
  }
}
