#include "widget.h"
#include <queue>

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
void Map::prepare(std::set<int> &prepared, glm::ivec2 pos, char code) {
  std::vector<glm::ivec2> adj = {
      glm::ivec2(pos.x + 1, pos.y),
      glm::ivec2(pos.x - 1, pos.y),
      glm::ivec2(pos.x, pos.y + 1),
      glm::ivec2(pos.x, pos.y - 1),
  };
  std::pair _pair(pos.x, pos.y);
  for (auto &a : adj) {
    if (is_valid(a.x, a.y) && data[a.x][a.y] == code) {
      // 有效的接壤同符号是待合并的标志
      // 现在找到这个集合
      for (auto s = 0; s < adj_block[code].size(); s++) {
        if (!prepared.contains(s)) {
          // 集合只添加一次
          auto i = std::find(adj_block[code][s].begin(),
                             adj_block[code][s].end(), std::pair(a.x, a.y));
          if (i != adj_block[code][s].end()) {
            // 找到了这个唯一的集合
            prepared.insert(s);
          }
        }
      }
    }
  }
}
void Map::merge_list(std::set<int> &prepared, char code) {
  // 将idx指定的某一个code下的list进行合并
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      heap;
  for (auto &i : prepared) {
    heap.push(std::pair(adj_block[code][i].size(), i));
  }
  while (heap.size() > 1) {
    std::pair _p_1 = heap.top();
    heap.pop();
    std::pair _p_2 = heap.top();
    heap.pop();
    adj_block[code][_p_2.second].merge(std::move(adj_block[code][_p_1.second]));
    heap.push(std::pair(adj_block[code][_p_2.second].size(), _p_2.second));
  }
}
void Map::find_remove(std::pair<int, int> _pair, char old_code) {
  for (auto s = adj_block[old_code].begin(); s != adj_block[old_code].end();
       s++) {
    auto it = std::find(s->begin(), s->end(), _pair);
    if (it != s->end()) {
      s->erase(it);
      if (s->size() == 0) {
        adj_block[old_code].erase(s);
      }
      break;
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
      // FIXME 清除不会将本来分裂的集合不会自动进行分裂,事实上我也没有任何处理手段
      /*
      ### -> #.#但是左右的#他们仍然是一个整体,如果增加一个检查是否连接的函数,似乎代价快赶上dfs了.所有的成本都快赶上了.但是我正在处理的符号并不是#,也就意味着我需要一个全局连通检查,不现实
      问题也不在于此,我大可以在处理该符号的时候检查旧符号是否连通,如果因为这个修改而不连通就将集合进行分裂,我感觉不如增加邻接连通矩阵,然后直接在矩阵上修改来得快dfs的问题就是没有引入连通.
      想要继续该方法必须要解决:迅速判断集合是否出现了分裂.然而最佳方法居然是路径搜索算法.
      所以最后就是只有一个方法了.
      */
      char old_code = data[pos_start.x][pos_start.y];
      if (old_code != code) {
        std::set<int> prepared;
        prepared.insert(adj_block[code].size());
        prepare(prepared, pos_start, code);
        data[pos_start.x][pos_start.y] = code;
        draw_tile(render, pos_start);
        std::pair _pair(pos_start.x, pos_start.y);
        find_remove(_pair, old_code);
        std::list<std::pair<int, int>> tmp;
        tmp.push_back(_pair);
        adj_block[code].push_back(tmp);
        merge_list(prepared, code);
      }
      begin_draw = false;
      break;
    }
    case 2: {
      const int min_x = std::min(pos_start.x, pos_end.x);
      const int min_y = std::min(pos_start.y, pos_end.y);
      const int max_x = std::max(pos_start.x, pos_end.x);
      const int max_y = std::max(pos_start.y, pos_end.y);
      std::list<std::pair<int, int>> tmp;
      char old_code = data[pos_start.x][pos_start.y];
      std::set<int> prepared;
      prepared.insert(adj_block[code].size());
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          glm::ivec2 point(i, j);
          if (i == min_x || i == max_x || j == min_y || j == max_y) {
            // 四周是和其他地方接壤的唯一可能点,当然还可以进一步优化,例如,中心点一定不接壤
            prepare(prepared, point, code);
          }
          // glm::ivec2 point = glm::ivec2(i, j);
          data[i][j] = code;
          // move_point(point);
          draw_tile(render, point);
          std::pair _pair(i, j);
          // delete point from old code set
          find_remove(_pair, old_code);
          // add to new code set
          tmp.push_back(_pair);
        }
      }
      adj_block[code].push_back(tmp);
      merge_list(prepared, code);
      begin_draw = false;
      break;
    }
    case 3: {
      char old_code = data[pos_start.x][pos_start.y];
      std::pair _pair = std::pair(pos_start.x, pos_start.y);
      if (old_code != code) {
        int idx = 0;
        std::set<int> prepared; // 之后待合并的列表
        for (int k = 0; k < adj_block[old_code].size(); k++) {
          auto _iter = std::find(adj_block[old_code][k].begin(),
                                 adj_block[old_code][k].end(), _pair);
          if (_iter != adj_block[old_code][k].end()) {
            idx = k;
            int length_old = adj_block[old_code][k].size();
            int length_code = 0;
            for (auto &it : adj_block[code]) {
              length_code += it.size();
            }
            if (length_code < length_old) {
              int i = 0;
              for (auto &s : adj_block[code]) {
                for (auto &_p : s) {
                  std::vector<std::pair<int, int>> adj;
                  if (is_valid(_p.first + 1, _p.second) &&
                      data[_p.first + 1][_p.second] == code) {
                    adj.emplace_back(_p.first + 1, _p.second);
                  }
                  if (is_valid(_p.first - 1, _p.second) &&
                      data[_p.first - 1][_p.second] == code) {
                    adj.emplace_back(_p.first - 1, _p.second);
                  }
                  if (is_valid(_p.first, _p.second + 1) &&
                      data[_p.first][_p.second + 1] == code) {
                    adj.emplace_back(_p.first, _p.second + 1);
                  }
                  if (is_valid(_p.first, _p.second - 1) &&
                      data[_p.first][_p.second - 1] == code) {
                    adj.emplace_back(_p.first, _p.second - 1);
                  }
                  if (std::find(adj_block[old_code][idx].begin(),
                                adj_block[old_code][idx].end(),
                                _p) != adj_block[old_code][idx].end()) {
                    prepared.insert(i);
                  }
                }
                i++;
              }
            } else {
              for (auto &_p : adj_block[old_code][idx]) {
                std::vector<std::pair<int, int>> adj;
                if (is_valid(_p.first + 1, _p.second) &&
                    data[_p.first + 1][_p.second] == code) {
                  adj.emplace_back(_p.first + 1, _p.second);
                }
                if (is_valid(_p.first - 1, _p.second) &&
                    data[_p.first - 1][_p.second] == code) {
                  adj.emplace_back(_p.first - 1, _p.second);
                }
                if (is_valid(_p.first, _p.second + 1) &&
                    data[_p.first][_p.second + 1] == code) {
                  adj.emplace_back(_p.first, _p.second + 1);
                }
                if (is_valid(_p.first, _p.second - 1) &&
                    data[_p.first][_p.second - 1] == code) {
                  adj.emplace_back(_p.first, _p.second - 1);
                }
                int i = 0;
                for (auto &s : adj_block[code]) {
                  for (auto &a : adj) {
                    if (std::find(s.begin(), s.end(), a) != s.end()) {
                      prepared.insert(i);
                      break;
                    }
                  }
                  i++;
                }
              }
            }
            for (auto _p : adj_block[old_code][k]) {
              data[_p.first][_p.second] = code;
              draw_tile(render, glm::ivec2(_p.first, _p.second));
            }
            break;
          }
        }
        prepared.insert(adj_block[code].size());
        adj_block[code].push_back(std::move(adj_block[old_code][idx]));
        // 保留位置,在此处进行删除
        adj_block[old_code].erase(adj_block[old_code].begin() + idx);
        merge_list(prepared, code);
      }
      begin_draw = false;
      break;
    }
    case 4: {
      // free paint
      glm::ivec2 point = get_grid();
      char old_code = data[point.x][point.y];
      if (old_code != code) {
        std::set<int> prepared;
        prepared.insert(adj_block[code].size());
        prepare(prepared, point, code);
        data[point.x][point.y] = code;
        draw_tile(render, point);
        std::pair _pair(point.x, point.y);
        find_remove(_pair, old_code);
        std::list<std::pair<int, int>> tmp;
        tmp.push_back(_pair);
        adj_block[code].push_back(tmp);
        merge_list(prepared, code);
      }
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
