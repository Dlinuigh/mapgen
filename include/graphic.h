#ifndef GRAPHIC_H
#define GRAPHIC_H
#include "data.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
class Graphic {
  std::map<std::string, SDL_Surface *> textures;
  std::map<char, SDL_Surface *> charmap;
  Json::Value image_doc = {};
  std::map<std::string, glm::ivec2> tileset = {};
  std::string cached_png_name = {}; // 正缓存的图片id
  int size{};                       // tile size
  Data &data;
  Graphic() : data(Data::getInstance()) { init(); }
  void load_images();
  std::map<std::string, int> images = {};
  // get png file image, the whole image. use id, not filename
  SDL_Surface *get_image(const std::string &name);
  void init() { load_images(); }

public:
  Graphic(Graphic const &) = delete;
  void operator=(Graphic const &) = delete;
  // draw tile from name.
  // 因为要用widget,所以应该返回一个texture.
  SDL_Surface *get_tile(const std::string &name, const std::string &tilename);
  SDL_Surface *get_char(char key, TTF_Font *font, SDL_Color fcolor);
  static Graphic &getInstance() {
    static Graphic instance;
    return instance;
  }
  ~Graphic() {
    for (auto &it : charmap) {
      SDL_DestroySurface(it.second);
    }
    for (auto &it : textures) {
      SDL_DestroySurface(it.second);
    }
  }
};
#endif
