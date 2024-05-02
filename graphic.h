#ifndef GRAPHIC_H
#define GRAPHIC_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>
#include <vector>

class point {
public:
    float x,y;
  point();
  point(float _x, float _y):x(_x), y(_y){}
};

class btn {
public:
  SDL_FRect pos;
  btn();
  btn(float x, float y, float w, float h) {
    pos.x = x;
    pos.y = y;
    pos.w = w;
    pos.h = h;
  }
};

class graphic {
  SDL_Renderer *render;
  std::vector<btn> tools;
  std::vector<bool> enable;
  std::vector<std::vector<point>> map;
  std::vector<std::vector<char>> data;
  SDL_Texture *tools_texture;
  SDL_Texture *cells;
  SDL_Texture *tool;
  TTF_Font *font;
  int width;
  int height;
  int col;
  int row;
  int area_start_col;
  int area_start_row;
  bool area;
  int layer;
  int sep;
  int tile;
  int margin;
  int tool_margin;
  int tool_sep;
  int tools_num;
  int cell_x;
  int cell_y;
  uint_fast8_t code;
  std::map<uint_fast8_t, SDL_Texture *> t_map;
  SDL_Texture* create_texture_from_char(char);
  void clear(){
    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(render);
  }
  void draw_data(int, int);
  void save();
  void cal_pos();
  bool is_valid(int, int);
  void dfs(int, int, std::vector<point>&, char);
  std::vector<point> find_adj_set(int, int);
public:
  graphic(int w, int h, int l) : col(w), row(h){
    layer = l;
    cal_pos();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_EVENTS);
    TTF_Init();
    std::string pwd = "/home/lion/Projects/farm/mapgen/";
    std::string font_name = "assets/Terminus.ttf";
    std::string tool_name = "assets/tool.png";
    std::string ttf_file_path = (pwd+font_name);
    std::string tool_file_path = (pwd+tool_name);
    font = TTF_OpenFont(ttf_file_path.c_str(), 16);
    SDL_Window *window =
        SDL_CreateWindow("Mapgen", width, height, SDL_WINDOW_BORDERLESS);
    render = SDL_CreateRenderer(window, "render", SDL_RENDERER_SOFTWARE);
    tools_texture =
        SDL_CreateTextureFromSurface(render, IMG_Load(tool_file_path.c_str()));
    cells = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888,
                            SDL_TEXTUREACCESS_TARGET, width, height);
    tool = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888,
                            SDL_TEXTUREACCESS_TARGET, width, tile+margin+tool_margin);
    SDL_SetRenderTarget(render, cells);
    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(render);
    SDL_SetRenderTarget(render, tool);
    SDL_SetRenderDrawColor(render, 255, 255, 255, 0);
    SDL_RenderClear(render);
  }
  void draw_ui();
  void draw_grid();
  void draw_tool();
  void handle(SDL_Event);
  void draw_tool_selected();
  void end() {
    SDL_SetRenderTarget(render, NULL);
    SDL_RenderTexture(render, cells, NULL, NULL);
    SDL_RenderPresent(render);
  }
  void begin() {
    SDL_SetRenderTarget(render, tool);
  }
  void transfer(){
    SDL_FRect dst = {(float)0, (float)0, (float)width, 1.0f*(tile+margin+tool_margin)};
    SDL_SetRenderTarget(render, cells);
    SDL_RenderTexture(render, tool, NULL, &dst);
  }
  void quit(){
    for(const auto &i: t_map){
      SDL_DestroyTexture(i.second);
    }
    SDL_DestroyTexture(tool);
    SDL_DestroyTexture(cells);
  }
  bool is_quit(SDL_Event event){
    return !(event.type == SDL_EVENT_KEY_DOWN && event.key.keysym.sym == SDLK_q && event.key.keysym.mod & SDL_KMOD_CTRL);
  }
};

#endif
