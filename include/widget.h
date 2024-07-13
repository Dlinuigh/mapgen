#ifndef WIDGET_H
#define WIDGET_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <glm/glm.hpp>
#include <string>
class Widget {
public:
  std::string desc; // 描述该控件的用处
  std::string text;
  std::function<void()> callback; // 点击触发的函数
  SDL_FRect area;   // 绝对区域,该位置需要后续的计算进行调整
  TTF_Font *font;   // 来自game管理的字体
  SDL_Color fcolor; // 字体颜色
  SDL_Surface *bg;  // 背景图片
  float ratio;
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
  void set_text(std::string _text);
};
class Check final : public Widget {
  bool activated=false;
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
#endif
