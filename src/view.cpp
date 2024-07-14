#include "view.h"
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
    for (const auto &it : children) {
      if (it->click()) {
        return true;
      }
    }
  }
  return false;
}
void Box::draw(SDL_Renderer *render, SDL_Event event) {
  if (bg != nullptr) {
    draw(render, event);
  }
  for (const auto &it : children) {
    it->draw(render, event);
  }
}
void Box::set_size() {
  // 设置box的大小，但是似乎不能整理孩子的大小。
  if (vertical) {
    area.w = child_size.x;
    for (auto &it : children) {
      area.h += it->area.h;
    }
    // for(auto &it: children){
    //   it->resize(child_size.x, vertical);
    // }
  } else {
    area.h = child_size.y;
    for (auto &it : children) {
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
  for (auto &it : children) {
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
void View::locate(){
  for(auto &it: children){
    locate_child(it.first, it.second);
  }
}
void View::locate_child(const std::shared_ptr<Widget> &child, Position position){
  // 根据大小与相对位置确定绝对位置
  glm::fvec2 pos;
  glm::fvec2 rd_corner_pos = {scr_size.x - child->area.w,
                              scr_size.y - child->area.h};
  glm::fvec2 c_lu_cor_pos = {scr_size.x / 2.0f - child->area.w / 2,
                             scr_size.y / 2.0f - child->area.h / 2};
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
  for (const auto &it : children) {
    if (it.first->click()) {
      return true;
    }
  }
  return false;
}
void View::draw(SDL_Renderer* render, SDL_Event event){
  for(auto &it: children){
    it.first->draw(render, event);
  }
}