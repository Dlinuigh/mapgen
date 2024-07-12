#include "graphic.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
void Graphic::load_images() {
  std::string img_fold = "../assets/image/";
  data.parse(img_fold, "image.json", image_doc);
  int i = 0;
  for (auto &it : image_doc) {
    std::string img_path = img_fold + it["file"].asString();
    SDL_Surface *loaded_img = IMG_Load(img_path.c_str());
    if (loaded_img == nullptr) {
      std::cerr << "Unable to load image " << img_path
                << "! SDL Error: " << SDL_GetError() << std::endl;
    } else {
      images[it["name"].asString()] = i; // record index to fast find.
      i++;
      textures[it["name"].asString()] = std::move(loaded_img);
    }
  }
}
SDL_Surface *Graphic::get_image(const std::string &name) {
  if (images.contains(name))
    return textures[name];
  else
    return nullptr;
}
SDL_Texture *Graphic::get_tile(SDL_Renderer *render, const std::string &name,
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
    SDL_FRect tile_src = {static_cast<float>(pos.x * size),
                          static_cast<float>(pos.y * size),
                          static_cast<float>(size), static_cast<float>(size)};
    SDL_Texture *src = SDL_CreateTextureFromSurface(render, sur);
    // caculator dst to replace nullptr;
    SDL_Texture *result = SDL_CreateTexture(
        render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, size, size);
    SDL_SetRenderTarget(render, result);
    SDL_RenderTexture(render, src, &tile_src, nullptr);
    return result;
  } else {
    return nullptr;
  }
}