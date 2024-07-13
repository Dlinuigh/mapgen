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
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_RenderTexture(render, texture, nullptr, &area);
}
void Widget::resize(float length, bool horizon) {
  // FIXME 这个函数目前并没有使用，然后这个处理，默认是靠左上，这样就不用修改area的xy
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
void Widget::set_desire_size(glm::fvec2 size) {
  area.w = size.x;
  area.h = size.y;
}
void Widget::set_text(std::string _text){
  text = std::move(_text);
}
void Check::set_check_texture(SDL_Surface *surface) { check_sign = surface; }
void Check::draw(SDL_Renderer *render, SDL_Event) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_RenderTexture(render, texture, nullptr, &area);
  SDL_DestroyTexture(texture);
  if (activated) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, check_sign);
    SDL_RenderTexture(render, texture, nullptr, &area);
    SDL_DestroyTexture(texture);
  }
}
bool Check::click() {
  if (in() && callback!=nullptr) {
    callback();
    return true;
  } else {
    return false;
  }
}
void Check::activate(){
  activated=!activated;
}