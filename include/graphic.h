#ifndef GRAPHIC_H
#define GRAPHIC_H
#include "data.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
class Graphic {
  SDL_Renderer *render;
  std::map<std::string, SDL_Surface *> textures;
  std::map<char, SDL_Texture *> charmap;
  std::map<std::string, SDL_Texture *> tilemap;
  Json::Value image_doc = {};
  std::map<std::string, glm::ivec2> tileset = {};
  std::string cached_png_name = {};
  int size{};
  Data &data;
  Graphic() : data(Data::getInstance()) { init(); }
  void load_images();
  std::map<std::string, int> images = {};
  SDL_Surface *get_image(const std::string &name);
  void init() { load_images(); }

public:
  Graphic(Graphic const &) = delete;
  void operator=(Graphic const &) = delete;
  SDL_Texture *get_tile(const std::string &name, const std::string &tilename);
  SDL_Texture *get_char(char key, TTF_Font *font, SDL_Color fcolor);
  void set_render(SDL_Renderer *_render) { render = _render; }
  static Graphic &getInstance() {
    static Graphic instance;
    return instance;
  }
  ~Graphic() {
    for (auto &it : charmap) {
      SDL_DestroyTexture(it.second);
    }
    for (auto &it : textures) {
      SDL_DestroySurface(it.second);
    }
    for (auto &it : tilemap) {

      SDL_DestroyTexture(it.second);
    }
  }
};
#endif
