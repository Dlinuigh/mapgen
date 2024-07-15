#include "graphic.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

void Graphic::load_images() {
  const std::string img_fold = "assets/image/";
  Data::parse(img_fold, "image.json", image_doc);
  int i = 0;
  for (auto &it : image_doc) {
    std::string img_path = img_fold + it["file"].asString();
    if (SDL_Surface *loaded_img = IMG_Load(img_path.c_str());
        loaded_img == nullptr) {
      std::cerr << "Unable to load image " << img_path
                << "! SDL Error: " << SDL_GetError() << std::endl;
    } else {
      images[it["name"].asString()] = i; // record index to fast find.
      i++;
      textures[it["name"].asString()] = loaded_img;
    }
  }
}

SDL_Surface *Graphic::get_image(const std::string &name) {
  if (images.contains(name))
    return textures[name];
  return nullptr;
}

SDL_Surface *Graphic::get_tile(const std::string &name,
                               const std::string &tilename) {
  if (images.contains(name)) {
    if (name != cached_png_name) {
      Json::Value img = image_doc[images[name]]; // the png image.
      cached_png_name = name;
      tileset.clear();
      for(auto &it:tilemap){
        SDL_DestroySurface(it.second);
      }
      size = img["size"].asInt();
      for (auto &it : img["images"]) {
        tileset[it["name"].asString()] =
            glm::ivec2(it["pos"][0].asInt(), it["pos"][1].asInt());
      }
    }
    if (tilemap.contains(tilename)) {
      return tilemap[tilename];
    }
    const glm::ivec2 pos = tileset[tilename];
    SDL_Surface *sur = textures[name];
    const SDL_Rect tile_src = {pos.x * size, pos.y * size, size, size};
    SDL_Surface *result =
        SDL_CreateSurface(size, size, SDL_PIXELFORMAT_ARGB8888);
    SDL_BlitSurface(sur, &tile_src, result, nullptr);
    tilemap[tilename] = result;
    return result;
  }
  return nullptr;
}

SDL_Surface *Graphic::get_char(const char key, TTF_Font *font,
                               const SDL_Color fcolor) {
  // FIXME 如果字符相同不会考虑字体、字体颜色
  if (charmap.contains(key))
    return charmap[key];
  char tmp[2];
  tmp[0] = key;
  tmp[1] = '\0';
  charmap[key] = TTF_RenderUTF8_Solid(font, tmp, fcolor);
  return charmap[key];
}
