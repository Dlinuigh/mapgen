#ifndef WIDGET_H
#define WIDGET_H
#include "graphic.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <glm/glm.hpp>
#include <list>
#include <set>
class Widget {
public:
  float ratio =
      1.0f; // 计算完成area.w与h才能开始计算ratio,对于一些组件可能会延后。
  SDL_Color fcolor = {};
  SDL_FRect area = {};
  SDL_Texture *bg = nullptr;
  std::function<void()> hover = nullptr;
  std::function<void()> press = nullptr;
  std::function<void()> release = nullptr;
  TTF_Font *font = nullptr;
  Widget() = default;
  virtual ~Widget() { SDL_DestroyTexture(bg); }
  virtual bool pressed(SDL_Event &);  // 返回值为了尽早结束循环
  virtual bool released(SDL_Event &); // 释放按键
  virtual bool hovering(SDL_Event &);
  virtual void draw(SDL_Renderer *, SDL_Event);
  virtual void locate(glm::fvec2);
  virtual void resize(glm::fvec2);
  // void resize(float, bool);
  [[nodiscard]] bool in() const;
};
class Check final : public Widget {
  bool activated = false;
  SDL_Texture *selected = nullptr;

public:
  Check(SDL_Texture *texture, SDL_Texture *_selected) : selected(_selected) {
    bg = texture;
    float w = 0.0f, h = 0.0f;
    SDL_GetTextureSize(texture, &w, &h);
    area.w = w;
    area.h = h;
    ratio = area.w / area.h;
  }
  ~Check() {
    if (selected != nullptr)
      SDL_DestroyTexture(selected);
  }
  bool pressed(SDL_Event &) override;
  void activate();
  void draw(SDL_Renderer *render, SDL_Event) override;
};
class Map final : public Widget {
  std::map<std::string, bool>
      buttons; // 按钮的状态,左键、右键、中键、侧键1、侧键2
  char keycode = ' ';
  bool begin_draw = false;
  bool already_selected_one = false;
  int tile_size = 0;
  int select_type = 0;
  int special_action = 0;
  glm::ivec2 size; // map grid size, col and row
  glm::ivec2 pos_start;
  glm::ivec2 pos_end;
  Graphic &graphic;
  SDL_Color bgcolor = {255, 255, 255, SDL_ALPHA_OPAQUE};
  SDL_Texture *grid;
  std::vector<std::vector<bool>> walked;
  std::map<char, std::vector<std::list<std::pair<int, int>>>>
      adj_block; // for fill function
  void clear_tile(SDL_Renderer *, SDL_FRect) const;
  void draw_char(SDL_Renderer *, SDL_FRect) const;
  void draw_grid(SDL_Renderer *) const;
  void generate_grid(SDL_Renderer *) const;
  void move_point(glm::ivec2 pos);
  void general_handle(int, char, char);
  void move_draw_set(SDL_Renderer *, char, char,
                     std::set<std::pair<int, int>> &);
  [[nodiscard]] bool is_valid(int, int) const;
  [[nodiscard]] SDL_FRect get_area(glm::ivec2) const;

public:
  std::vector<std::vector<char>> data;
  Map(const glm::ivec2 _size, const float tilesize, SDL_Renderer *render)
      : tile_size(tilesize), size(_size), graphic(Graphic::getInstance()) {
    int w = size.x * tilesize;
    int h = size.y * tilesize;
    area.w = w;
    area.h = h;
    data.assign(size.y, std::vector(size.x, ' '));
    walked.assign(size.y, std::vector(size.x, false));
    adj_block[' '].assign(1, {});
    for (int i = 0; i < size.x; i++) {
      for (int j = 0; j < size.y; j++) {
        adj_block[' '][0].push_back(std::pair(i, j));
      }
    }
    bg = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
                           SDL_TEXTUREACCESS_TARGET, w, h);
    grid = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
                             SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetRenderDrawColor(render, bgcolor.r, bgcolor.g, bgcolor.b,
                           SDL_ALPHA_TRANSPARENT);
    SDL_SetTextureBlendMode(bg, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(grid, SDL_BLENDMODE_BLEND);
    SDL_Texture *texture = SDL_GetRenderTarget(render);
    SDL_SetRenderTarget(render, bg);
    SDL_RenderClear(render);
    SDL_SetRenderTarget(render, grid);
    SDL_RenderClear(render);
    generate_grid(render);
    SDL_SetRenderTarget(render, texture);
  }
  void draw_tile(SDL_Renderer *, glm::ivec2);
  [[nodiscard]] glm::ivec2 get_grid() const;
  void draw(SDL_Renderer *, SDL_Event) override;
  bool pressed(SDL_Event &) override;
  bool hovering(SDL_Event &) override;
  bool released(SDL_Event &) override;
  void set_key(char c);
  void set_function(int, int);
  ~Map() override { SDL_DestroyTexture(grid); }
};
class Label final : public Widget {
  std::string text;

public:
  Label() = default;
  void set_text(std::string, SDL_Renderer *);
  void draw(SDL_Renderer *, SDL_Event) override;
};
#endif
