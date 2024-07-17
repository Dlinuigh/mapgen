#ifndef VIEW_H
#define VIEW_H
#include "utils.h"
#include "widget.h"
#include <memory>
#include <tuple>
#include <vector>

class Box final : public Widget {
  std::vector<std::shared_ptr<Widget>> children;
  bool vertical = false; // 竖直排列？
  glm::fvec2 child_size = {0, 0};

public:
  explicit Box(const bool _vertical) : vertical(_vertical) {}

  void push_back(const std::shared_ptr<Widget> &child);

  bool pressed(SDL_Event &) override;

  void draw(SDL_Renderer *render, SDL_Event event) override;

  void set_size();

  void locate(glm::fvec2 position) override;
};
struct Compare {
  const int column = 0;
  Compare(int col) : column(col) {}
  bool operator()(const std::tuple<int, int, std::shared_ptr<Widget>, Position> &row_1, const std::tuple<int, int, std::shared_ptr<Widget>, Position> &row_2) {
    if (column == 0)
      return std::get<0>(row_1) < std::get<0>(row_2);
    if (column == 1)
      return std::get<1>(row_1) < std::get<1>(row_2);
    return false;
  }
};
class View final {
  void locate_child(const std::shared_ptr<Widget> &child, Position position) const;

  glm::ivec2 scr_size;

public:
  explicit View(glm::ivec2 _size) : scr_size(_size) {}

  std::vector<std::tuple<int, int, std::shared_ptr<Widget>, Position>> children;

  bool pressed(SDL_Event &);
  bool released(SDL_Event &);
  bool hovering(SDL_Event &);

  void push_back(const std::shared_ptr<Widget> &, Position, int, int);

  void draw(SDL_Renderer *render, const SDL_Event &event);

  void locate();

  void set_screen_size(glm::ivec2 size) { scr_size = size; }
};
#endif
