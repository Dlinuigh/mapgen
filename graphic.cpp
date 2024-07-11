#include "graphic.h"
#include <cmath>
#include <iostream>
#include <json/json.h>

void graphic::draw_grid() {
  
  for (int i = 0; i <= col; i++) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    if(i%5 == 0&&i!=0&&i!=col){
      SDL_SetRenderDrawColor(render, 0, 0, 255, SDL_ALPHA_OPAQUE);
    }
    if(i%10 == 0&&i!=0&&i!=col){
      SDL_SetRenderDrawColor(render, 255, 0, 255, SDL_ALPHA_OPAQUE);
    }
    SDL_FRect line = {cell_x * 1.0f + (sep + tile) * i, 0.0f + cell_y,
                      1.0f * sep, (tile + sep) * row * 1.0f + sep};
    SDL_RenderFillRect(render, &line);
  }
  for (int j = 0; j <= row; j++) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    if(j%5 == 0&&j!=0&&j!=row){
      SDL_SetRenderDrawColor(render, 0, 0, 255, SDL_ALPHA_OPAQUE);
    }
    if(j%10 == 0&&j!=0&&j!=row){
      SDL_SetRenderDrawColor(render, 255, 0, 255, SDL_ALPHA_OPAQUE);
    }
    SDL_FRect line = {0.0f + cell_x, cell_y * 1.0f + (sep + tile) * j,
                      (tile + sep) * col * 1.0f + sep, 1.0f * sep};
    SDL_RenderFillRect(render, &line);
  }
}
void graphic::draw_tool() {
  for (int i = 0; i < tools_num; i++) {
    SDL_FRect src = {16.0f * i, 0.0f, 16.0f, 16.0f};
    SDL_RenderTexture(render, tools_texture, &src, &tools[i].pos);
  }
}
void graphic::draw_tool_selected() {
  if (enable[0]) {
    SDL_FRect src = {0.0f, 16.0f, 16.0f, 16.0f};
    SDL_RenderTexture(render, tools_texture, &src, &tools[0].pos);
  }
  if (enable[1]) {
    SDL_FRect src = {0.0f, 16.0f, 16.0f, 16.0f};
    SDL_RenderTexture(render, tools_texture, &src, &tools[1].pos);
  }
  if (enable[2]) {
    SDL_FRect src = {0.0f, 16.0f, 16.0f, 16.0f};
    SDL_RenderTexture(render, tools_texture, &src, &tools[2].pos);
  }
  if (enable[3]) {
    SDL_FRect src = {0.0f, 16.0f, 16.0f, 16.0f};
    SDL_RenderTexture(render, tools_texture, &src, &tools[3].pos);
  }
}
void graphic::save() {
  Json::Value document;
  document["layer"] = this->layer;
  Json::Value m(Json::arrayValue);
  for (size_t j = 0; j < data[0].size(); j++) {
    std::string line;
    for (size_t i = 0; i < data.size(); i++) {
      line.push_back(data[i][j]);
    }
    m.append(line);
  }
  document["main"] = m;
  Json::StreamWriterBuilder builder;
  std::string output = Json::writeString(builder, document);
  std::cout << output << std::endl;
}

void graphic::cal_pos() {
  tool_sep = 2;
  tool_margin = 2;
  margin = 64;
  tile = 32;
  tools_num = 5;
  sep = 2;
  int min_width =
      (tile + sep) * tools_num + tool_margin * 2 + margin * 2 - tool_sep;
  width = margin * 2 + (tile + sep) * col + sep;
  int tool_offset_x = margin + tool_margin;
  int tool_offset_y = margin + tool_margin;
  height = (tile + sep) * row + sep + tool_margin * 2 + margin * 2 + tile;
  int offset_x = margin + sep;
  int offset_y = margin + tool_margin * 2 + sep + tile;
  cell_y = offset_y - sep;
  if (width > min_width) {
    tool_sep = (width - margin * 2 - tool_margin * 2 - tools_num * tile) /
               (tools_num - 1);
    cell_x = offset_x - sep;
    for (int i = 0; i < col; i++) {
      std::vector<point> t;
      for (int j = 0; j < row; j++) {
        point c((float)(offset_x + (sep + tile) * i),
                (float)(offset_y + (sep + tile) * j));
        t.push_back(c);
      }
      map.push_back(t);
    }
  } else {
    width = min_width;
    offset_x = (width - (tile + sep) * col + sep) / 2;
    cell_x = offset_x;
    for (int i = 0; i < col; i++) {
      std::vector<point> t;
      for (int j = 0; j < row; j++) {
        point c((float)(offset_x + (sep + tile) * i),
                (float)(offset_y + (sep + tile) * j));
        t.push_back(c);
      }
      map.push_back(t);
    }
  }
  for (int i = 0; i < tools_num; i++) {
    btn b((float)(tool_offset_x + (tile + tool_sep) * i), (float)tool_offset_y,
          1.0f * tile, 1.0f * tile);
    tools.push_back(b);
    enable.push_back(false);
  }
  data.resize(col, std::vector<char>(row, ' '));///data 初始化为空格。
  sign.resize(col, std::vector<bool>(row, false));
}

bool graphic::is_valid(int x, int y) {
  return x >= 0 && x < col && y >= 0 && y < row;
}

void graphic::dfs(int x, int y, std::vector<point> &adj_set, char target) {
  if (!is_valid(x, y)){
    return;
  }else if((sign[x][y] == true)) {
    return;
  }else if(data[x][y] != target){
    return ;
  }

  // 将当前点加入相邻点集合
  adj_set.push_back(point(x, y));

  // 标记当前点已访问
  data[x][y] = enable[2] ? ' ' : (char)code;
  sign[x][y] = true;

  // 定义四个方向的偏移量
  int dx[] = {-1, 1, 0, 0};
  int dy[] = {0, 0, -1, 1};

  // 对四个方向进行搜索
  for (int i = 0; i < 4; ++i) {
    int nx = x + dx[i];
    int ny = y + dy[i];
    dfs(nx, ny, adj_set, target);
  }
}

std::vector<point> graphic::find_adj_set(int c, int r) {
  std::vector<point> adj_set;
  char target = data[c][r];
  dfs(c, r, adj_set, target);
  return adj_set;
}

void graphic::handle(SDL_Event event) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      int x = event.button.x;
      int y = event.button.y;
      for (size_t i = 0; i < tools.size(); i++) {
        SDL_FRect area = tools[i].pos;
        if (x >= area.x && x < area.w + area.x && y >= area.y &&
            y < area.h + area.y) {
          if (i == 0) {
            if (!enable[0]) {
              enable[1] = enable[0];
              enable[3] = enable[0];
            }
            enable[0].flip();
          } else if (i == 1) {
            if (!enable[1]) {
              enable[0] = enable[1];
              enable[3] = enable[1];
            }
            enable[1].flip();
          } else if (i == 2) {
            enable[2].flip();
          } else if (i == 3) {
            if (!enable[3]) {
              enable[0] = enable[3];
              enable[1] = enable[3];
            }
            enable[3].flip();
          } else if (i == 4) {
            save();
          }
        }
      }
      if (x >= cell_x && x < width - cell_x && y >= cell_y &&
          y < height - margin) {
        int c = (int)floor(1.0f * (x - cell_x) / (tile + sep));
        int r = (int)floor(1.0f * (y - cell_y) / (tile + sep));
        if (enable[1] && !area) {
          area_start_col = c;
          area_start_row = r;
          area = true;
        } else if (enable[1] && area) { // area function 
          int start_x = std::min(area_start_col, c);
          int end_x = std::max(area_start_col, c);
          int start_y = std::min(area_start_row, r);
          int end_y = std::max(area_start_row, r);
          for (int i = start_x; i <= end_x; i++) {
            for (int j = start_y; j <= end_y; j++) {
              data[i][j] = enable[2] ? ' ' : (char)code;
              draw_data(i, j);
            }
          }
          area = false;
        } else if (enable[0]) { // point function
          data[c][r] = enable[2] ? ' ' : (char)code;
          draw_data(c, r);
        } else if (enable[3]) { // fill function
          sign.assign(col, std::vector<bool>(row, false));
          std::vector<point> adj_set = find_adj_set(c, r);
          for (size_t i = 0; i < adj_set.size(); i++) {
            int c_t = adj_set[i].x;
            int r_t = adj_set[i].y;
            draw_data(c_t, r_t);
          }
        }
      }
    }
  }
  if (event.type == SDL_EVENT_KEY_DOWN) {
    if (event.key.mod & SDL_KMOD_SHIFT) {
      if (event.key.key >= (int)'a' &&
          event.key.key <= (int)'z') {
        code = event.key.key - 'a' + 'A';
      } else {
        switch (event.key.key) {
        case '`':
          code = '~';
          break;
        case '1':
          code = '!';
          break;
        case '2':
          code = '@';
          break;
        case '3':
          code = '#';
          break;
        case '4':
          code = '$';
          break;
        case '5':
          code = '%';
          break;
        case '6':
          code = '^';
          break;
        case '7':
          code = '&';
          break;
        case '8':
          code = '*';
          break;
        case '9':
          code = '(';
          break;
        case '0':
          code = ')';
          break;
        case '-':
          code = '_';
          break;
        case '=':
          code = '+';
          break;
        case '[':
          code = '{';
          break;
        case ']':
          code = '}';
          break;
        case '\\':
          code = '|';
          break;
        case ';':
          code = ':';
          break;
        case '\'':
          code = '\"';
          break;
        case ',':
          code = '<';
          break;
        case '.':
          code = '>';
          break;
        case '/':
          code = '?';
          break;
        }
      }
    } else if (event.key.mod & SDL_KMOD_CAPS) {
      code = event.key.key - 'a' + 'A';
    } else {
      code = event.key.key;
    }
  }
}

SDL_Texture *graphic::create_texture_from_char(char c) {
  char ch[2]="";
  ch[0] = c;
  ch[1] = '\0';
  SDL_Surface *surface = TTF_RenderText_Solid(font, ch, {0, 0, 0, SDL_ALPHA_OPAQUE});
  SDL_Texture *result = SDL_CreateTextureFromSurface(render, surface);
  SDL_DestroySurface(surface);
  return result;
}

void graphic::draw_data(int i, int j) {
  auto it = t_map.find(data[i][j]);
  SDL_Texture *text;
  if (it != t_map.end()) {
    text = it->second;
  } else {
    text = create_texture_from_char(data[i][j]);
    t_map[data[i][j]] = text;
  }
  SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_FRect dst = {map[i][j].x, map[i][j].y, 1.0f * tile, 1.0f * tile};
  SDL_RenderFillRect(render, &dst);
  if(data[i][j]!=' ')
    SDL_RenderTexture(render, text, NULL, &dst);
}

// void graphic::draw_blueprint(SDL_Event event) {
//   int x = event.button.x;
//   int y = event.button.y;
//   if (x >= cell_x && x < width - cell_x && y >= cell_y && y < height - margin) {
//     if (enable[1]) {
//       SDL_SetRenderDrawColor(render, 0x46, 0xe3, 0xd5, SDL_ALPHA_OPAQUE);
//       int c = (int)floor(1.0f * (x - cell_x) / (tile + sep));
//       int r = (int)floor(1.0f * (y - cell_y) / (tile + sep));
//       int start_col = std::min(area_start_col, c);
//       int end_col = std::max(area_start_col, c);
//       int start_row = std::min(area_start_row, r);
//       int end_row = std::max(area_start_row, r);
//       float start_x = map[start_col][start_row].x;
//       float start_y = map[start_col][start_row].y;
//       float end_x = map[end_col][end_row].x;
//       float end_y = map[end_col][end_row].y;
//       SDL_FRect dst = {start_x, start_y, end_x - start_x, end_y - start_y};
//       SDL_RenderFillRect(render, &dst);
//     }
//   }
// }