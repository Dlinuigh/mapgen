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
void Cell::set_value(char c){
  // for external function callback to change value to a variable value.
  value = c;
}
bool Cell::click(){
  if (in() && callback!=nullptr) {
    callback();
    return true;
  } else {
    return false;
  }
}
void Cell::draw(SDL_Renderer* render, SDL_Event){
  bg = TTF_RenderUTF8_Solid(font, &value, fcolor);
  float w = bg->w;
  float h = bg->h;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bg);
  SDL_FRect dst = {area.x+(area.w-w)/2, area.y+(area.h-h)/2, w, h};
  SDL_RenderTexture(render, texture, nullptr, &dst);
  SDL_DestroyTexture(texture);
}