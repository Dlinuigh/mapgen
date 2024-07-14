#ifndef FONT_H
#define FONT_H

#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>

class Font {
private:
  Font() { TTF_Init(); }

  // Private constructor for singleton pattern
  std::map<std::pair<std::string, int>, TTF_Font *> flut;

public:
  Font(Font const &) = delete;

  void operator=(Font const &) = delete;

  static Font &getInstance() {
    static Font instance;
    return instance;
  }

  TTF_Font *get_font(const std::string &_font, int fsize) {
    const auto key = std::make_pair(_font, fsize);
    if (!flut.contains(key)) {
      const std::string ffile = std::string("../assets/font/") + _font;
      TTF_Font *font = TTF_OpenFont(ffile.c_str(), fsize);
      flut[key] = font;
    }
    return flut[key];
  }

  ~Font() {
    for (auto &it : flut) {
      TTF_CloseFont(it.second);
    }
  }
};

#endif
