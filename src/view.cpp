#include "view.h"
#include <algorithm>
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
bool Box::pressed(SDL_Event &event) {
  if (in()) {
    return std::ranges::any_of(
        children, [&event](auto it) { return it->pressed(event); });
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
  // TODO 设置box的大小，但是似乎不能整理孩子的大小。
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
void Box::locate(glm::vec2 position) {
  area.x = position.x;
  area.y = position.y;
  glm::vec2 offset = position;
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
void View::push_back(const std::shared_ptr<Widget> &child, Position position,
                     int click_pior, int draw_pior) {
  children.emplace_back(click_pior, draw_pior, child, position);
}
void View::locate() {
  for (auto &[pressed, draw, fst, snd] : children) {
    locate_child(fst, snd);
  }
}
void View::locate_child(const std::shared_ptr<Widget> &child,
                        const Position position) const {
  // 根据大小与相对位置确定绝对位置
  glm::vec2 pos = {};
  glm::vec2 rd_corner_pos = {static_cast<float>(scr_size.x) - child->area.w,
                              static_cast<float>(scr_size.y) - child->area.h};
  glm::vec2 c_lu_cor_pos = {
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
bool View::pressed(SDL_Event &event) {
  std::sort(children.begin(), children.end(), Compare(0));
  return std::ranges::any_of(
      children, [&event](auto it) { return std::get<2>(it)->pressed(event); });
}
bool View::released(SDL_Event &event) {
  std::sort(children.begin(), children.end(), Compare(0));
  return std::ranges::any_of(
      children, [&event](auto it) { return std::get<2>(it)->released(event); });
}
bool View::hovering(SDL_Event &event) {
  std::sort(children.begin(), children.end(), Compare(0));
  return std::ranges::any_of(
      children, [&event](auto it) { return std::get<2>(it)->hovering(event); });
}
void View::draw(SDL_Renderer *render, const SDL_Event &event) {
  std::sort(children.begin(), children.end(), Compare(1));
  for (const auto &child : children) {
    std::get<2>(child)->draw(render, event);
  }
}
