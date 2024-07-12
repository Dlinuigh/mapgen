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
  SDL_RenderTexture(render, bg, nullptr, &area);
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
void Check::set_check_texture(SDL_Texture *texture) { check_sign = texture; }
void Check::draw(SDL_Renderer *renderer, SDL_Event) {
  SDL_RenderTexture(renderer, bg, nullptr, &area);
  if (activated) {
    SDL_RenderTexture(renderer, check_sign, nullptr, &area);
  }
}
bool Check::click() {
  activated = !activated;
  return true;
}