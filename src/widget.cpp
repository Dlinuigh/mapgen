#include "widget.h"
bool Widget::in() const {
  glm::fvec2 mouse;
  SDL_GetMouseState(&mouse.x, &mouse.y);
  return mouse.x > area.x && mouse.x <= area.x + area.w && mouse.y > area.y && mouse.y <= area.y + area.h;
}
void Widget::draw(SDL_Renderer *, SDL_Event) {}
bool Widget::pressed(SDL_Event &) { return false; }
bool Widget::released(SDL_Event &) { return false; }
bool Widget::hovering(SDL_Event &) { return false; }
// void Widget::resize(const float length, const bool horizon) {
//   if (horizon) {
//     area.w = length;
//     area.h = length / ratio;
//   } else {
//     area.h = length;
//     area.w = length * ratio;
//   }
// }
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
  const SDL_FRect draw_text_area = {dst.x + (dst.w - w) / 2, dst.y + (dst.h - h) / 2, w, h};
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
void Map::move_point(glm::ivec2 pos) {
  char code = special_action == -1 ? ' ' : keycode;
  std::set<std::pair<int, int>> tmp;
  std::pair pair = std::pair(pos.x, pos.y);
  tmp.insert(pair);
  std::vector<glm::ivec2> points = {
      glm::ivec2(pos.x + 1, pos.y),
      glm::ivec2(pos.x - 1, pos.y),
      glm::ivec2(pos.x, pos.y + 1),
      glm::ivec2(pos.x, pos.y - 1),
  };
  char old_code = data[pos.x][pos.y];
  for (auto &it : adj_block[old_code]) {
    if (it.contains(pair)) {
      auto iter = std::find(it.begin(), it.end(), pair);
      it.erase(iter);
    }
  }
  adj_block[code].push_back(tmp);
  auto p = adj_block[code].rbegin();
  for (auto it = adj_block[code].rbegin() + 1; it != adj_block[code].rend(); it++) {
    for (auto point : points) {
      if (is_valid(point.x, point.y) && data[point.x][point.y] == code &&
          it->contains(std::pair(point.x, point.y))) {
        if (p->size() > it->size()) {
          for (auto &_point : *it) {
            p->insert(_point);
          }
          adj_block[code].erase(it.base() - 1);
        } else {
          for (auto &_point : *p) {
            it->insert(_point);
          }
          adj_block[code].erase(p.base() - 1);
          p = it;
        }
      }
    }
  }
  data[pos_start.x][pos_start.y] = code;
}
void Map::move_draw_set(SDL_Renderer *render, char code, std::set<std::pair<int, int>> &s_p) {
  for (auto &_point : s_p) {
    std::vector<std::pair<int, int>> points = {
        std::pair(_point.first + 1, _point.second),
        std::pair(_point.first - 1, _point.second),
        std::pair(_point.first, _point.second + 1),
        std::pair(_point.first, _point.second - 1),
    };
    for (auto &_p : points) {
      if (is_valid(_p.first, _p.second) && data[_p.first][_p.second] != code) {
        if (adj_block.contains(code) || adj_block[code].size()==0) {
          for (auto &t : s_p) {
            data[t.first][t.second] = code;
            draw_tile(render, glm::ivec2(t.first, t.second));
          }
          adj_block[code].push_back(std::move(s_p));
          return;
        } else {
          for (auto &it : adj_block[code]) {
            if (it.contains(_p)) {
              for (auto &_tmp : s_p) {
                data[_tmp.first][_tmp.second] = code;
                draw_tile(render, glm::ivec2(_tmp.first, _tmp.second));
                it.insert(_tmp);
              }
              return;
            }
          }
        }
      }
    }
  }
}
void Map::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, bg);
  if (begin_draw) {
    switch (select_type) {
    case 1: {
      move_point(pos_start);
      draw_tile(render, pos_start);
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
          glm::ivec2 point = glm::ivec2(i, j);
          move_point(point);
          draw_tile(render, point);
        }
      }
      begin_draw = false;
      break;
    }
    case 3: {
      char code = special_action == -1 ? ' ' : keycode;
      char old_code = data[pos_start.x][pos_start.y];
      std::pair pair = std::pair(pos_start.x, pos_start.y);
      for (auto it = adj_block[old_code].begin(); it != adj_block[old_code].end(); it++) {
        if (it->contains(pair)) {
          move_draw_set(render, code, *it);
          adj_block[old_code].erase(it);
          break;
        }
      }
      begin_draw = false;
      break;
    }
    case 4: {
      // free paint
      glm::ivec2 point = get_grid();
      move_point(point);
      draw_tile(render, point);
      break;
    }
      // case 4: {
      //   const int min_x = std::min(pos_start.x, pos_end.x);
      //   const int min_y = std::min(pos_start.y, pos_end.y);
      //   const int max_x = std::max(pos_start.x, pos_end.x);
      //   const int max_y = std::max(pos_start.y, pos_end.y);
      // }
      // case 5: {
      //   const int min_x = std::min(pos_start.x, pos_end.x);
      //   const int min_y = std::min(pos_start.y, pos_end.y);
      //   const int max_x = std::max(pos_start.x, pos_end.x);
      //   const int max_y = std::max(pos_start.y, pos_end.y);
      // }
      // case 6: {
      //   const int min_x = std::min(pos_start.x, pos_end.x);
      //   const int min_y = std::min(pos_start.y, pos_end.y);
      //   const int max_x = std::max(pos_start.x, pos_end.x);
      //   const int max_y = std::max(pos_start.y, pos_end.y);
      // }
      // case 7: {
      //   // saw
      // }
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
      // Rect function
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

  return {std::max(0, std::min(x, size.x - 1)), std::max(0, std::min(y, size.y - 1))};
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
