#ifndef VIEW_H
#define VIEW_H
#include "utils.h"
#include "widget.h"
#include <map>
#include <memory>
#include <vector>
// general contrainer header file
class Box : public Widget {
  // view中定义大致位置
  std::vector<std::shared_ptr<Widget>> children;
  bool vertical; // 竖直排列？
  glm::fvec2 child_size;

public:
  explicit Box(bool _vertical) : vertical(_vertical) {}
  void push_back(const std::shared_ptr<Widget> &child);
  bool click() override;
  void draw(SDL_Renderer *render, SDL_Event event) override;
  // 因为box的定义依赖子组件，构造的时候不放进所有的子组件，所以必须要一个函数后续处理
  void set_size();
  void locate(glm::fvec2 position) override;
};
class View {
  virtual void locate_child(const std::shared_ptr<Widget> &child,
                            Position position);
  glm::ivec2 scr_size;

public:
  View(glm::ivec2 _size) : scr_size(_size) {}
  std::map<SDL_Keycode, std::function<void()>> shotcut;
  std::vector<std::pair<std::shared_ptr<Widget>, Position>> children;
  // 锚点的设计是这样的，对于child的position也是screen的position.比如child的左侧中心对准screen的左侧中心
  virtual bool click();
  virtual void push_back(const std::shared_ptr<Widget> &child,
                         Position position);
  virtual void draw(SDL_Renderer *render, SDL_Event event);
  virtual void locate();
  void set_screen_size(glm::ivec2 size){
    scr_size = size;
  }
};
#endif