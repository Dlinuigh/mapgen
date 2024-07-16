#include "widget.h"
bool Widget::in() const {
  glm::fvec2 mouse;
  SDL_GetMouseState(&mouse.x, &mouse.y);
  return mouse.x > area.x && mouse.x <= area.x + area.w && mouse.y > area.y && mouse.y <= area.y + area.h;
}
void Widget::draw(SDL_Renderer*,SDL_Event){}
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
void Map::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, bg);
  if (begin_draw) {
    switch (select_type) {
    case 1: {
      auto pair = std::pair(pos_start.x,pos_start.y);
      adj_block[data[pos_start.x][pos_start.y]].erase(pair);
      adj_block[special_action == -1 ? ' ' : keycode].insert(pair);
      data[pos_start.x][pos_start.y] = special_action == -1 ? ' ' : keycode;
      draw_tile(render, pos_start);
      break;
    }
    case 2: {
      const int min_x = std::min(pos_start.x, pos_end.x);
      const int min_y = std::min(pos_start.y, pos_end.y);
      const int max_x = std::max(pos_start.x, pos_end.x);
      const int max_y = std::max(pos_start.y, pos_end.y);
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          auto point = std::pair(i, j);
          adj_block[data[i][j]].erase(point);
          adj_block[special_action == -1 ? ' ' : keycode].insert(point);
          data[i][j] = special_action == -1 ? ' ' : keycode;
          draw_tile(render, glm::ivec2(i,j));
        }
      }
      break;
    }
    case 3: {
      adj_block[special_action == -1 ? ' ' : keycode].insert(
          adj_block[data[pos_start.x][pos_start.y]].begin(),
          adj_block[data[pos_start.x][pos_start.y]].end());
      adj_block.erase(data[pos_start.x][pos_start.y]);
      for (const auto &it : adj_block[special_action == -1 ? ' ' : keycode]) {
        data[it.first][it.second] = special_action == -1 ? ' ' : keycode;
        draw_tile(render, glm::ivec2(it.first,it.second));
      }
      break;
    }
    case 4: {
      // free paint
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
    begin_draw = false;
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
bool Map::hovering(SDL_Event &) {}
bool Map::released(SDL_Event &) {}
void Map::set_key(const char c) { keycode = c; }
void Map::set_function(int _select, int _special){
  select_type = _select;
  special_action = _special;
}
bool Map::is_valid(const int x, const int y) const {
  return x >= 0 && y >= 0 && x < size.x && y < size.y;
}
glm::ivec2 Map::get_grid() const {
  glm::fvec2 mouse_pos;
  SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
  return {static_cast<int>((mouse_pos.x - area.x) / tile_size),
          static_cast<int>((mouse_pos.y - area.y) / tile_size)};
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
