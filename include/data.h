#ifndef DATA_H
#define DATA_H
#include <json/json.h>
class Data {
  // map data store in a dim 2 char.
  // std::vector<std::vector<char>> map;
  // single instance
  Data() {}
  Data(Data const &) = delete;
  void operator=(Data const &) = delete;
public:
  void parse(std::string dir, std::string file, Json::Value &doc);
  static Data &getInstance() {
    static Data instance;
    return instance;
  }
};
#endif
