#include "graphic.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

void Graphic::load_images() {
  const std::string img_fold = "../assets/image/";
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
  else
    return nullptr;
}

SDL_Surface *Graphic::get_tile(const std::string &name,
                               const std::string &tilename) {
  glm::ivec2 pos;
  SDL_Surface *sur;
  if (images.contains(name)) {
    if (name != cached_png_name) {
      Json::Value img = image_doc[images[name]]; // the png image.
      cached_png_name = name;
      tileset.clear();
      size = img["size"].asInt();
      for (auto &it : img["images"]) {
        tileset[it["name"].asString()] =
            glm::ivec2(it["pos"][0].asInt(), it["pos"][1].asInt());
      }
    }
    pos = tileset[tilename];
    sur = textures[name];
    SDL_Rect tile_src = {pos.x * size, pos.y * size, size, size};
    SDL_Surface *result =
        SDL_CreateSurface(size, size, SDL_PIXELFORMAT_ARGB8888);
    SDL_BlitSurface(sur, &tile_src, result, nullptr);
    return result;
  } else {
    return nullptr;
  }
}

SDL_Surface *Graphic::get_char(char key, TTF_Font *font, SDL_Color fcolor) {
  if (charmap.contains(key))
    return charmap[key];
  else {
    char tmp[2];
    tmp[0] = key;
    tmp[1] = '\0';
    charmap[key] = TTF_RenderUTF8_Solid(font, tmp, fcolor);
    return charmap[key];
  }
}
