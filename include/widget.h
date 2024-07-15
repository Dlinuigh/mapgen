#ifndef WIDGET_H
#define WIDGET_H
#include "graphic.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <glm/glm.hpp>

class Widget {
public:
  std::function<void()> callback = nullptr; // 点击触发的函数
  SDL_FRect area = {}; // 绝对区域,该位置需要后续的计算进行调整
  TTF_Font *font = nullptr;  // 来自game管理的字体
  SDL_Color fcolor = {};     // 字体颜色
  SDL_Surface *bg = nullptr; // 背景图片
  float ratio = 1.0f;
  // w/h的比例，用来保持长宽比例地缩放，这个时候总有一个变量不需要统一
  // 还有一点：缩放后不会进行拉伸，要不然会导致变形
  Widget() = default;

  virtual ~Widget() = default;
  // 函数用来将内容绘制到texture上，并且不会获取任何texture的信息，只是复制。
  virtual void draw(SDL_Renderer *, SDL_Event);

  // 点击，需要首先判断是否在区域内，然后具有触发函数。
  virtual bool click();

  // 判断鼠标是否在控件里面
  [[nodiscard]] bool in() const;

  // 确定大小,专治奇奇怪怪的内容的组件,第二个参数是该长度是否是水平变量
  void resize(float length, bool horizon);

  // 确定位置
  virtual void locate(glm::fvec2 position);

  virtual void set_desire_size(glm::fvec2 size);
};

class Check final : public Widget {
  bool activated = false;
  SDL_Surface *check_sign = nullptr;

public:
  explicit Check(SDL_Surface *surface) {
    bg = surface;
    // 获取背景材质的大小并计算需求大小。在创建时必须要设置bg的信息。
    // 图片保持原始长宽比例才行
    const glm::fvec2 size(surface->w, surface->h);
    area.w = size.x;
    area.h = size.y;
    ratio = area.w / area.h;
  }

  void activate();

  void set_check_texture(SDL_Surface *surface);

  void draw(SDL_Renderer *render, SDL_Event) override;

  bool click() override;
};

class Cell final : public Widget {
  char value = ' ';

public:
  Cell() = default;

  explicit Cell(char _init_char) : value(_init_char) {};

  bool click() override;

  void set_value(char c);

  void draw(SDL_Renderer *, SDL_Event) override;
};

class Map final : public Widget {
  std::vector<std::vector<bool>> sign;
  char code = ' ';
  glm::ivec2 size;
  glm::ivec2 start_pos;
  glm::ivec2 end_pos;
  bool left_button_down;
  std::vector<bool> function_state = std::vector(11, false); // 0,1,2,3
  float tile_size;
  Graphic &graphic;
  SDL_Texture *grid;

  [[nodiscard]] bool is_valid(int, int) const;

  void dfs(int, int, std::vector<glm::ivec2> &adj_set, char target);

  [[nodiscard]] SDL_FRect get_area(glm::ivec2) const;

  void draw_char(SDL_Renderer *, SDL_FRect) const;

  SDL_Texture *map_view;
  bool begin_draw = false;
  bool already_select_one = false;
  SDL_Color bgcolor = {255, 255, 255, SDL_ALPHA_OPAQUE};

  void clear_tile(SDL_Renderer *, SDL_FRect) const;

  void draw_grid(SDL_Renderer *) const;

  void generate_grid(SDL_Renderer *) const;

public:
  std::vector<std::vector<char>> data;

  Map(const glm::ivec2 _size, const float tilesize, SDL_Renderer *render)
      : size(_size), start_pos(), end_pos(), tile_size(tilesize),
        graphic(Graphic::getInstance()) {
    area.w = static_cast<float>(size.x) * tilesize;
    area.h = static_cast<float>(size.y) * tilesize;
    data.assign(size.y, std::vector<char>(size.x, ' '));
    sign.assign(size.y, std::vector<bool>(size.x, false));
    map_view = SDL_CreateTexture(
        render, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET,
        static_cast<int>(area.w), static_cast<int>(area.h));
    grid = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
                             SDL_TEXTUREACCESS_TARGET, static_cast<int>(area.w),
                             static_cast<int>(area.h));
    SDL_SetRenderDrawColor(render, bgcolor.r, bgcolor.g, bgcolor.b, SDL_ALPHA_TRANSPARENT);
    SDL_SetTextureBlendMode(map_view, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(grid, SDL_BLENDMODE_BLEND);
    SDL_Texture *texture = SDL_GetRenderTarget(render);
    SDL_SetRenderTarget(render, map_view);
    SDL_RenderClear(render);
    // SDL_SetRenderDrawColor(render, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
    SDL_SetRenderTarget(render, grid);
    SDL_RenderClear(render);
    // function below will set target.
    generate_grid(render);
    SDL_SetRenderTarget(render, texture);
  }
  void draw_tile(SDL_Renderer *, glm::ivec2);
  [[nodiscard]] glm::ivec2 get_grid() const;

  void draw(SDL_Renderer *, SDL_Event) override;

  bool click() override;

  // 每一次检测到key的更换需要调用一次
  void set_key(char c);

  // 功能的修改需要调用一次
  void set_function(const std::vector<bool> &func);

  ~Map() override {
    SDL_DestroyTexture(map_view);
    SDL_DestroyTexture(grid);
  }
};

class Label final : public Widget {
  std::string text;
  SDL_Surface *surface{};
  Graphic &graphic;

public:
  Label() : graphic(Graphic::getInstance()) {};

  // call before draw
  void set_text(std::string);

  void draw(SDL_Renderer *, SDL_Event) override;

  ~Label() override { SDL_DestroySurface(surface); }
};
#endif
