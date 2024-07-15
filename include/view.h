#ifndef VIEW_H
#define VIEW_H
#include "utils.h"
#include "widget.h"
#include <memory>
#include <vector>

class Box final : public Widget {
  std::vector<std::shared_ptr<Widget>> children;
  bool vertical=false; // 竖直排列？
  glm::fvec2 child_size = {0, 0};

public:
  explicit Box(const bool _vertical) : vertical(_vertical) {}

  void push_back(const std::shared_ptr<Widget> &child);

  bool click() override;

  void draw(SDL_Renderer *render, SDL_Event event) override;

  void set_size();

  void locate(glm::fvec2 position) override;
};

class View final {
  void locate_child(const std::shared_ptr<Widget> &child, Position position);

  glm::ivec2 scr_size;

public:
  explicit View(glm::ivec2 _size) : scr_size(_size) {}

  std::vector<std::pair<std::shared_ptr<Widget>, Position>> children;

  bool click();

  void push_back(const std::shared_ptr<Widget> &child, Position position);

  void draw(SDL_Renderer *render, SDL_Event event);

  void locate();

  void set_screen_size(glm::ivec2 size) { scr_size = size; }
};
#endif
