#ifndef WIDGET_H
#define WIDGET_H
#include "graphic.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <glm/glm.hpp>
class Widget {
public:
  std::function<void()> callback; // 点击触发的函数
  SDL_FRect area;   // 绝对区域,该位置需要后续的计算进行调整
  TTF_Font *font;   // 来自game管理的字体
  SDL_Color fcolor; // 字体颜色
  SDL_Surface *bg;  // 背景图片
  float ratio = 1.0f;
  // w/h的比例，用来保持长宽比例地缩放，这个时候总有一个变量不需要统一
  // 还有一点：缩放后不会进行拉伸，要不然会导致变形
  Widget() : area({}), font(nullptr), fcolor({}), bg(nullptr) {}
  virtual ~Widget() = default;
  // 函数用来将内容绘制到texture上，并且不会获取任何texture的信息，只是复制。
  virtual void draw(SDL_Renderer *, SDL_Event);
  // 点击，需要首先判断是否在区域内，然后具有触发函数。
  virtual bool click();
  // 判断鼠标是否在控件里面
  bool in() const;
  // 确定大小,专治奇奇怪怪的内容的组件,第二个参数是该长度是否是水平变量
  void resize(float length, bool horizon);
  // 确定位置
  virtual void locate(glm::fvec2 position);
  virtual void set_desire_size(glm::fvec2 size);
};
class Check final : public Widget {
  bool activated = false;
  SDL_Surface *check_sign;

public:
  explicit Check(SDL_Surface *surface) {
    bg = surface;
    // 获取背景材质的大小并计算需求大小。在创建时必须要设置bg的信息。
    // 图片保持原始长宽比例才行
    glm::fvec2 size(surface->w, surface->h);
    area.w = size.x;
    area.h = size.y;
    ratio = area.w / area.h;
  }
  void activate();
  void set_check_texture(SDL_Surface *surface);
  void draw(SDL_Renderer *renderer, SDL_Event) override;
  bool click() override;
  ~Check() = default;
};
class Cell : public Widget {
  char value = ' ';

public:
  Cell() = default;
  Cell(char _init_char) : value(_init_char) {};
  bool click() override;
  void set_value(char c);
  void draw(SDL_Renderer *, SDL_Event) override;
};
class Map : public Widget {
  std::vector<std::vector<char>> data;
  std::vector<std::vector<bool>> sign;
  char code=' ';
  glm::ivec2 size;
  glm::ivec2 start_pos;
  glm::ivec2 end_pos;
  std::vector<bool> function_state={false, false, false, false}; // 0,1,2,3
  float tile_size;
  Graphic &graphic;
  glm::ivec2 get_grid();
  bool is_valid(int, int);
  void dfs(int, int, std::vector<glm::ivec2> &adj_set, char target);
  SDL_FRect get_area(glm::ivec2);
  void draw_tile(SDL_Renderer*, glm::ivec2);
  SDL_Texture *map_view;
public:
  Map(glm::ivec2 _size, float tilesize, SDL_Renderer* render) : size(_size), tile_size(tilesize), graphic(Graphic::getInstance()) {
    area.w = size.x*tilesize;
    area.h = size.y*tilesize;
    data.assign(size.y, std::vector<char>(size.x, ' '));
    sign.assign(size.y, std::vector<bool>(size.x, false));
    map_view = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
                                 SDL_TEXTUREACCESS_TARGET, area.w,
                                 area.h);
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_Texture* texture = SDL_GetRenderTarget(render);
    SDL_SetRenderTarget(render, map_view);
    SDL_RenderClear(render);
    SDL_SetRenderTarget(render, texture);
  }
  void draw(SDL_Renderer *, SDL_Event) override;
  bool click() override;
  // 每一次检测到key的更换需要调用一次
  void set_key(char c);
  // 功能的修改需要调用一次
  void set_function(std::vector<bool> func);
};
#endif
