#include "view.h"

#include <algorithm>
#include <ranges>

void Box::push_back(const std::shared_ptr<Widget> &child) {
  children.push_back(child);
  // 获取组件的最小大小要求
  if (vertical) {
    // 竖直排列组件需要保持横向宽度统一，对于各组件的竖直长度可以自由调整
    if (child_size.x < child->area.w)
      child_size.x = child->area.w;
  } else {
    // 比如，context多行文本与entry的单行文本的结合
    if (child_size.y < child->area.h)
      child_size.y = child->area.h;
  }
}

bool Box::click() {
  if (in()) {
    return std::ranges::any_of(children, [](auto it) { return it->click(); });
  }
  return false;
}

void Box::draw(SDL_Renderer *render, const SDL_Event event) {
  if (bg != nullptr) {
    Widget::draw(render, event);
  }
  for (const auto &it : children) {
    it->draw(render, event);
  }
}

void Box::set_size() {
  // 设置box的大小，但是似乎不能整理孩子的大小。
  if (vertical) {
    area.w = child_size.x;
    for (const auto &it : children) {
      area.h += it->area.h;
    }
    // for(auto &it: children){
    //   it->resize(child_size.x, vertical);
    // }
  } else {
    area.h = child_size.y;
    for (const auto &it : children) {
      area.w += it->area.w;
    }
    // for(auto &it: children){
    //   it->resize(child_size.y, vertical);
    // }
  }
}

void Box::locate(glm::fvec2 position) {
  area.x = position.x;
  area.y = position.y;
  glm::fvec2 offset = position;
  for (const auto &it : children) {
    if (vertical) {
      it->locate({position.x, offset.y});
      offset.y += it->area.h;
    } else {
      it->locate({offset.x, position.y});
      offset.x += it->area.w;
    }
  }
}

// 该函数只能是子组件的大小被准确的确定了才能调用
void View::push_back(const std::shared_ptr<Widget> &child, Position position) {
  // emplace可以避免使用构造makepair,而是直接在向量末尾进行内存上的构造。
  children.emplace_back(child, position);
}

void View::locate() {
  for (auto &[fst, snd] : children) {
    locate_child(fst, snd);
  }
}

void View::locate_child(const std::shared_ptr<Widget> &child,
                        const Position position) {
  // 根据大小与相对位置确定绝对位置
  glm::fvec2 pos = {};
  glm::fvec2 rd_corner_pos = {static_cast<float>(scr_size.x) - child->area.w,
                              static_cast<float>(scr_size.y) - child->area.h};
  glm::fvec2 c_lu_cor_pos = {
      static_cast<float>(scr_size.x) / 2.0f - child->area.w / 2,
      static_cast<float>(scr_size.y) / 2.0f - child->area.h / 2};
  switch (position) {
  case LU_CORNER:
    pos = {0, 0};
    break;
  case RU_CORNER:
    pos = {rd_corner_pos.x, 0};
    break;
  case LD_CORNER:
    pos = {0, rd_corner_pos.y};
    break;
  case RD_CORNER:
    pos = rd_corner_pos;
    break;
  case U_SIDE:
    pos = {c_lu_cor_pos.x, 0};
    break;
  case D_SIDE:
    pos = {c_lu_cor_pos.x, rd_corner_pos.y};
    break;
  case L_SIDE:
    pos = {0, c_lu_cor_pos.y};
    break;
  case R_SIDE:
    pos = {rd_corner_pos.x, c_lu_cor_pos.y};
    break;
  case CENTER:
    pos = c_lu_cor_pos;
    break;
  }
  child->locate(pos);
}

bool View::click() {
  return std::ranges::any_of(children,
                             [](auto it) { return it.first->click(); });
}

void View::draw(SDL_Renderer *render, const SDL_Event event) {
  for (const auto &child : children | std::ranges::views::keys) {
    child->draw(render, event);
  }
}
