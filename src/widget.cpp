#include "widget.h"
#include <random>
bool Widget::in() const {
  glm::vec2 mouse;
  SDL_GetMouseState(&mouse.x, &mouse.y);
  return mouse.x > area.x && mouse.x <= area.x + area.w && mouse.y > area.y &&
         mouse.y <= area.y + area.h;
}
void Widget::draw(SDL_Renderer *, SDL_Event) {}
bool Widget::pressed(SDL_Event &) { return false; }
bool Widget::released(SDL_Event &) { return false; }
bool Widget::hovering(SDL_Event &) { return false; }
void Widget::locate(const glm::vec2 position) {
  area.x = position.x;
  area.y = position.y;
}
void Widget::resize(const glm::vec2 size) {
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
  if (special_action == -2) {
    if (!selected.contains(point))
      selected.insert(point);
  } else {
    data[point.x][point.y] = code;
    draw_tile(render, point);
  }
  walked[point.x][point.y] = true;
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
        walked[a.x][a.y] = true;
        if (special_action == -2) {
          if (!selected.contains(a))
            selected.insert(a);
        } else {
          draw_tile(render, a);
          data[a.x][a.y] = code;
        }
      }
    }
  }
}
std::vector<glm::ivec2> Map::circle(int num_points) {
  // FIXME 使用等距弧度不够精准
  std::vector<glm::ivec2> points;

  // 判断是否能构成矩形
  if (pos_start == pos_end) {
    return points;
  }

  // 计算中心点和半轴
  glm::ivec2 center((pos_start.x + pos_end.x) / 2,
                    (pos_start.y + pos_end.y) / 2);
  double half_width = std::abs(pos_start.x - pos_end.x) / 2.0;
  double half_height = std::abs(pos_start.y - pos_end.y) / 2.0;

  // 判断是否是正方形
  bool isSquare = std::abs(half_width - half_height) < 1e-6; // 允许微小误差
  double radius_x, radius_y;
  if (isSquare) {
    radius_x = radius_y = half_width; // 正方形的情况下，两个半轴相等
  } else {
    radius_x = half_width;
    radius_y = half_height;
  }

  // 生成椭圆或圆形的点
  double angle_step = 2 * M_PI / num_points;
  for (int i = 0; i < num_points; ++i) {
    double angle = i * angle_step;
    int x = static_cast<int>(center.x + radius_x * std::cos(angle));
    int y = static_cast<int>(center.y + radius_y * std::sin(angle));

    glm::ivec2 point(x, y);

    // 检查点是否有效
    if (is_valid(point.x, point.y)) {
      points.push_back(point);
    }
  }

  return points;
}
void Map::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *target = SDL_GetRenderTarget(render);
  SDL_SetRenderTarget(render, bg);
  char code = special_action == -1 ? ' ' : keycode;
  if (begin_draw) {
    switch (select_type) {
      // TODO 这几个选取与选取功能的配合直接存放到一个数组里面为当前的选取点.
      // TODO 移动的配合就是将里面的所有点移动鼠标移动的距离
      // TODO 引入上色能力
      // TODO 上下层功能将上下层的某一层进行虚化调整透明度.然后和该层进行叠加.
    case 1: {
      char old_code = data[pos_start.x][pos_start.y];
      if (old_code != code) {
        if (special_action == -2) {
          if (!selected.contains(pos_start))
            selected.insert(pos_start);
        } else {
          data[pos_start.x][pos_start.y] = code;
          draw_tile(render, pos_start);
        }
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
            if (special_action == -2) {
              if (!selected.contains(point))
                selected.insert(point);
            } else {
              data[i][j] = code;
              draw_tile(render, point);
            }
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
        if (special_action == -2) {
          if (!selected.contains(point))
            selected.insert(point);
        } else {
          data[point.x][point.y] = code;
          draw_tile(render, point);
        }
      }
      break;
    }
    case 5: {
      std::vector<glm::ivec2> points = circle(100);
      for (auto &p : points) {
        if (special_action == -2) {
          if (!selected.contains(p))
            selected.insert(p);
        } else {
          data[p.x][p.y] = code;
          draw_tile(render, p);
        }
      }
      begin_draw = false;
      break;
    }
    case 6: {
      const int min_x = std::min(pos_start.x, pos_end.x);
      const int min_y = std::min(pos_start.y, pos_end.y);
      const int max_x = std::max(pos_start.x, pos_end.x);
      const int max_y = std::max(pos_start.y, pos_end.y);
      for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
          if (data[i][j] != code &&
              (i == min_x || i == max_x || j == min_y || j == max_y)) {
            glm::ivec2 point(i, j);
            if (special_action == -2) {
              if (!selected.contains(point))
                selected.insert(point);
            } else {
              data[i][j] = code;
              draw_tile(render, point);
            }
          }
        }
      }
      begin_draw = false;
      break;
    }
    case 7: {
      // FIXME
      // 解决了旧问题，但是由于代码写得不够通用所以引出了新的问题,暂时去做其他功能
      glm::ivec2 vector = pos_end - pos_start;
      glm::ivec2 old[2];
      if (vector.x != 0) {
        float k = 1.0f * vector.y / vector.x;
        int sign = (pos_end.x > pos_start.x) - (pos_end.x < pos_start.x);
        float step = 1 / sqrt(1 + k * k) * sign;
        old[0] = glm::ivec2(-1, -1);
        old[1] = pos_start;
        float i;
        for (i = pos_start.x; std::abs(i - pos_end.x) > std::abs(step);
             i += step) {
          float j = ((i - pos_start.x) * k + pos_start.y);
          glm::ivec2 point((int)i, (int)j);
          if ((point.x == old[1].x && old[0].y == old[1].y) ||
              (point.y == old[1].y && old[0].x == old[1].x)) {
            old[1] = point;
          } else {
            if (special_action == -2) {
              if (!selected.contains(old[1]))
                selected.insert(old[1]);
            } else {
              data[old[1].x][old[1].y] = code;
              draw_tile(render, old[1]);
            }
            old[0] = old[1];
            old[1] = point;
          }
        }
        float j = ((i - pos_start.x) * k + pos_start.y);
        glm::ivec2 point((int)i, (int)j);
        if (special_action == -2) {
          if (!selected.contains(point))
            selected.insert(point);
          if (!selected.contains(old[1]))
            selected.insert(old[1]);
          if (!selected.contains(pos_end))
            selected.insert(pos_end);
        } else {
          data[point.x][point.y] = code;
          draw_tile(render, point);
          data[old[1].x][old[1].y] = code;
          draw_tile(render, old[1]);
          data[pos_end.x][pos_end.y] = code;
          draw_tile(render, pos_end);
        }
      } else {
        int step = (pos_end.y > pos_start.y) - (pos_end.y < pos_start.y);
        for (int j = pos_start.y; j != pos_end.y; j += step) {
          glm::ivec2 point(pos_start.x, j);
          if (special_action == -2) {
            if (!selected.contains(point))
              selected.insert(point);
          } else {
            data[pos_start.x][j] = code;
            draw_tile(render, point);
          }
        }
        data[pos_end.x][pos_end.y] = code;
        draw_tile(render, pos_end);
      }
      begin_draw = false;
      break;
    }
    case 8: {
      glm::ivec2 point = get_grid();
      if (point.x != pos_start.x) {
        float mu = 0, sigma = 0.5;
        std::normal_distribution<float> distribution(mu, sigma);
        glm::vec2 noise = {0, 0};
        // noise.x = distribution(generator);
        noise.y = distribution(generator);
        glm::vec2 tmp = noise + (glm::vec2)point;
        glm::ivec2 p = glm::ivec2(fmin(size.x, fmax(0, tmp.x)),
                                  fmin(size.y, fmax(0, tmp.y)));
        char old_code = data[p.x][p.y];
        if (old_code != code) {
          if (special_action == -2) {
            if (!selected.contains(p))
              selected.insert(p);
          } else {
            data[p.x][p.y] = code;
            draw_tile(render, p);
          }
        }
      }
      pos_start = point;
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
    // 注意这里是在下方的选择区域，所以如果特殊功能开启就不会因为点击而修改而是会一直触发
    if (special_action == -2) {
      // TODO choose 使用清除功能会自动清空selected
      printf("%d\n", selected.size());
      // selected.clear();
    } else if (special_action == -3) {
      // TODO move 移动后会在移动功能结束清空selected
    } else if (special_action == -1) {
      // 清除功能
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
    if (select_type == 4 || select_type == 8) {
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
  glm::vec2 mouse_pos;
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
