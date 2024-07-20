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

void Map::move_point(glm::ivec2 pos) {
  // FIXME 没有正常找到邻接的集合加入，而是创建了多个邻接的集合，本来可以合并的
  char code = special_action == -1 ? ' ' : keycode;
  // // 创建了一个新的集合，并将修改的点加入了新的集合中
  // std::set<std::pair<int, int>> tmp;
  // std::pair pair = std::pair(pos.x, pos.y);
  // tmp.insert(pair);
  // char old_code = data[pos.x][pos.y];
  // for (auto it = adj_block[old_code].begin(); it !=
  // adj_block[old_code].end();
  //      it++) {
  //   if (it->contains(pair)) {
  //     auto iter = std::find(it->begin(), it->end(), pair);
  //     it->erase(iter);
  //     if (it->size() == 0) {
  //       adj_block[old_code].erase(it);
  //     }
  //   }
  // }
  // adj_block[code].push_back(tmp);
  // //
  // 遍历该点的四周查看是否存在相连的相同字符集合，如果存在就将少的合并到多的集合中
  // std::vector<glm::ivec2> points = {
  //     glm::ivec2(pos.x + 1, pos.y),
  //     glm::ivec2(pos.x - 1, pos.y),
  //     glm::ivec2(pos.x, pos.y + 1),
  //     glm::ivec2(pos.x, pos.y - 1),
  // };
  // auto p = adj_block[code].rbegin();
  // for (auto it = adj_block[code].rbegin() + 1; it != adj_block[code].rend();
  //      it++) {
  //   for (auto point : points) {
  //     // 当前的集合存在这个周围点
  //     if (is_valid(point.x, point.y) && data[point.x][point.y] == code &&
  //         it->contains(std::pair(point.x, point.y))) {
  //       if (p->size() > it->size()) {
  //         for (auto &_point : *it) {
  //           p->insert(_point);
  //         }
  //         adj_block[code].erase(it.base() - 1);
  //       } else {
  //         for (auto &_point : *p) {
  //           it->insert(_point);
  //         }
  //         adj_block[code].erase(p.base() - 1);
  //         p = it;
  //       }
  //     }
  //   }
  // }
  // data[pos_start.x][pos_start.y] = code;
  char old_code = data[pos.x][pos.y];
  std::list<std::pair<int, int>> li;
  li.push_back(std::pair(pos.x, pos.y));
  adj_block[old_code].push_back(li);
  for (auto &it : adj_block[old_code]) {
    auto _iter = std::find(it.begin(), it.end(), std::pair(pos.x, pos.y));
    if (_iter != it.end()) {
      it.erase(_iter);
      break;
    }
  }
  general_handle(adj_block[old_code].size() - 1, code, old_code);
}

void Map::general_handle(int idx, char code, char old_code) {
  // 希望旧的点应该先独立出来建立一个在旧符号下的列表，并设置idx是他的位置
  // idx是adj_block的vector里面的索引，使用迭代器超级麻烦
  int length_old = adj_block[old_code][idx].size();
  int length_code = 0;
  for (auto &it : adj_block[code]) {
    length_code += it.size();
  }
  // FIXME 解决了上次的问题，但是新的问题是，他会把其他字符进行清空
  std::vector<int> prepared; // 之后待合并的列表
  if (length_code < length_old) {
    int i = 0;
    for (auto &s : adj_block[code]) {
      for (auto &_p : s) {
        std::vector<std::pair<int, int>> adj;
        if (is_valid(_p.first + 1, _p.second) &&
            data[_p.first + 1][_p.second] != code) {
          adj.emplace_back(_p.first + 1, _p.second);
        }
        if (is_valid(_p.first - 1, _p.second) &&
            data[_p.first - 1][_p.second] != code) {
          adj.emplace_back(_p.first - 1, _p.second);
        }
        if (is_valid(_p.first, _p.second + 1) &&
            data[_p.first][_p.second + 1] != code) {
          adj.emplace_back(_p.first, _p.second + 1);
        }
        if (is_valid(_p.first, _p.second - 1) &&
            data[_p.first][_p.second - 1] != code) {
          adj.emplace_back(_p.first, _p.second - 1);
        }
        if (std::find(adj_block[old_code][idx].begin(),
                      adj_block[old_code][idx].end(),
                      _p) != adj_block[old_code][idx].end()) {
          prepared.push_back(i);
        }
      }
      i++;
    }
  } else {
    for (auto &_p : adj_block[old_code][idx]) {
      std::vector<std::pair<int, int>> adj;
      if (is_valid(_p.first + 1, _p.second) &&
          data[_p.first + 1][_p.second] != code) {
        adj.emplace_back(_p.first + 1, _p.second);
      }
      if (is_valid(_p.first - 1, _p.second) &&
          data[_p.first - 1][_p.second] != code) {
        adj.emplace_back(_p.first - 1, _p.second);
      }
      if (is_valid(_p.first, _p.second + 1) &&
          data[_p.first][_p.second + 1] != code) {
        adj.emplace_back(_p.first, _p.second + 1);
      }
      if (is_valid(_p.first, _p.second - 1) &&
          data[_p.first][_p.second - 1] != code) {
        adj.emplace_back(_p.first, _p.second - 1);
      }
      int i = 0;
      for (auto &s : adj_block[code]) {
        for (auto &a : adj) {
          if (std::find(s.begin(), s.end(), a) != s.end()) {
            prepared.push_back(i);
            break;
          }
        }
        i++;
      }
    }
  }
  if (prepared.size() != 0) {
    adj_block[code][prepared[0]].merge(std::move(adj_block[old_code][idx]));
    adj_block[old_code].erase(adj_block[old_code].begin() + idx);
    for (int i = 1; i < prepared.size(); i++) {
      adj_block[code][prepared[0]].merge(
          std::move(adj_block[code][prepared[i]]));
    }
    std::sort(prepared.begin(), prepared.end(), std::greater<int>());
    for (int i = 0; i < prepared.size() - 1; i++) {
      adj_block[code].erase(adj_block[code].begin() + prepared[i]);
    }
  } else {
    adj_block[code].push_back(std::move(adj_block[old_code][idx]));
    adj_block[old_code].erase(adj_block[old_code].begin() + idx);
  }
}

void Map::move_draw_set(SDL_Renderer *render, char code, char old_code,
                        std::set<std::pair<int, int>> &s_p) {

  // walked.assign(size.y, std::vector(size.x, false));
  // TODO 更高层次的抽象避免更多的分类情况
  /*
  对于这个问题，目前的分类情况已经让我很烦，单个点与集合之间的差异没有那么明显，但是却又不可以进行合并，我对此认为是目前的set不能够处理这样的问题，使用链表吧，虽然在cpp里面链表可能快要消失了，但是邻接表形式的数据存储可以相当轻松解决这些问题。首先我们创建一条625个元素的链表，当使用选择绘制的功能时，会从链表里面挑出在一起的一些或者一个点组合成新的链表。链表的优点在于不用像set一样逐个插入，毕竟我也不需要set的额外功能。
  另一个方法是，利用图里面的边，首先最开始所有的邻接点之间有边相连，如果一个点修改了值，那么他与周围不相同的都不相连。而如果在周围出现了相同值，那么会建立与他的相连的边。这样就可以使用邻接表了。邻接表的维护：唯一的问题是建立边的过程，首先需要判断边的类型，是否要断开，然后建立相同点之间的边的连接。
  处理的通用算法是：点变成集合。这样就只有集合之间的关系。而集合而言：
  首先要判断该集合中点的个数与将要变成的符号的全体集合之间的点的个数，处理更少的一方。相对的区别就是一个是一个集合，另一个是多个集合。一个集合遍历其中的点的周边，这样会得到一个集合的轮廓的概念，对于轮廓里面的点如果属于某个将变成的符号的集合，那么认为这两个集合需要合并，依次进行检查，找到所有需要和该集合进行合并的集合。最后执行集合的合并。由于cpp没有提供union函数，所以在这我们可以始终将更少的集合转移到更多的集合中来减少插入转移的次数？按照这个理论是否是移动次数50%？
  chatgpt说使用贪心算法，那么他要求我真的需要将所有的待合并的集合进行汇总后在进行合并操作。
  然后对于多个集合的点的数量更少的情况，对于集合依次进行遍历，但是这次的轮廓只有一个集合可以接壤，但是确是一样的操作，就是标记该集合为将合并集合。这样我们最终应该是得到一些待合并集合的指针或者是迭代器。使得这个问题变成了通用问题。然后其中的理论依据就是，如果之前的集合之间没有接壤，那么最终的结果就是各个集合与一个集合的接壤结果的并集。但是垃圾cpp没有合并集合的操作。
  无序链表插入很快、判断是否存在n,删除n
  集合插入logn,插入很多n*logn,判断是否存在logn,删除logn
  */
  // int length_code = 0, length_old = 0;
  // for (auto &it : adj_block[code]) {
  //   length_code += it.size();
  // }
  // length_old = s_p.size();
  // if (length_code < length_old) {
  //   for (auto it = adj_block[code].begin(); it != adj_block[code].end();
  //   it++) {
  //     for (auto _p = it->begin(); _p != it->end(); _p++) {
  //     }
  //   }
  // } else {
  //   for (auto &_point : s_p) {
  // FIXME 这个会导致某些点被重复访问
  /*
    如何快速知道那些边缘点，不属于该集合的点，或者只是避免点的重复访问就可以获得性能提升，
    首先可以根据data快速确定能否遍历，但是data会被重复访问，所以类似于walked这样的结构可能是合理的，类似的还有对于s_p增加一个等长度的vector,但是导致的问题是，需要判断点是否在s_p中。
    使用walked矩阵出现的问题是，每次进行该过程需要对矩阵进行重置一次。但是似乎代价可接受
    事情是这样的，我可以通过判断不同的集合的点的数量确定应该从code的集合开始查还是从old的
  */
  // std::vector<std::pair<int, int>> points = {
  //     std::pair(_point.first + 1, _point.second),
  //     std::pair(_point.first - 1, _point.second),
  //     std::pair(_point.first, _point.second + 1),
  //     std::pair(_point.first, _point.second - 1),
  /*
  pixelorama的斜角相连是不对等的，填充时可以斜角相连成块，而另一侧则是里外不相连的，尽管他们的斜角相连了。
  .#.
  #.#
  .#.
  上面的情况下，按理说对中心点进行填充，5个.斜角是相连的，但是pixelorama只会对中心点填充。并且如果你对单个#填充是不会影响其余的，哦好像是我之前的版本。
  */
  //     };
  //     for (auto &_p : points) {
  //       if (is_valid(_p.first, _p.second) &&
  //           data[_p.first][_p.second] != code &&
  //           walked[_p.first][_p.second] == false) {
  //         if (!adj_block.contains(code) || adj_block[code].size() == 0) {
  //           for (auto &t : s_p) {
  //             data[t.first][t.second] = code;
  //             draw_tile(render, glm::ivec2(t.first, t.second));
  //           }
  //           adj_block[code].push_back(std::move(s_p));
  //           return;
  //         } else {
  //           for (auto &it : adj_block[code]) {
  //             if (it.contains(_p)) {
  //               for (auto &_tmp : s_p) {
  //                 data[_tmp.first][_tmp.second] = code;
  //                 draw_tile(render, glm::ivec2(_tmp.first, _tmp.second));
  //                 it.insert(_tmp);
  //               }
  //               return;
  //             }
  //           }
  //         }
  //         walked[_p.first][_p.second] = true;
  //       }
  //     }
  //   }
  // }
}

void Map::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, bg);
  char code = special_action == -1 ? ' ' : keycode;
  if (begin_draw) {
    switch (select_type) {
    case 1: {
      data[pos_start.x][pos_start.y] = code;
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
      std::set<std::pair<int, int>> tmp_set;
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          glm::ivec2 point = glm::ivec2(i, j);
          data[i][j] = code;
          move_point(point);
          draw_tile(render, point);
        }
      }
      begin_draw = false;
      break;
    }
    case 3: {
      char old_code = data[pos_start.x][pos_start.y];
      std::pair pair = std::pair(pos_start.x, pos_start.y);
      // for (auto it = adj_block[old_code].begin();
      //      it != adj_block[old_code].end(); it++) {
      //   // if (it->contains(pair)) {
      //   //   move_draw_set(render, code, old_code, *it);
      //   //   adj_block[old_code].erase(it);
      //   //   break;
      //   // }

      // }
      for (int i = 0; i < adj_block[old_code].size(); i++) {
        auto _iter = std::find(adj_block[old_code][i].begin(),
                               adj_block[old_code][i].end(), pair);
        if (_iter != adj_block[old_code][i].end()) {
          for (auto _p : adj_block[old_code][i]) {
            data[_p.first][_p.second] = code;
            draw_tile(render, glm::ivec2(_p.first, _p.second));
          }
          general_handle(i, code, old_code);
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
