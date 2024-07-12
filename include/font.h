#ifndef FONT_H
#define FONT_H

#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>

class Font {
private:
  Font() {} // Private constructor for singleton pattern
  Font(Font const &) = delete;
  void operator=(Font const &) = delete;
  std::map<std::pair<std::string, int>, TTF_Font *> flut;

public:
  static Font &getInstance() {
    static Font instance;
    return instance;
  }

  TTF_Font *get_font(const std::string &_font, int fsize) {
    auto key = std::make_pair(_font, fsize);
    if (flut.find(key) == flut.end()) {
      std::string ffile = std::string("../assets/font/") + _font;
      TTF_Font *font = TTF_OpenFont(_font.c_str(), fsize);
      flut[key] = font;
    }
    return flut[key];
  }
};

#endif