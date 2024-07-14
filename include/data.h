#ifndef DATA_H
#define DATA_H
#include <json/json.h>

class Data {
  // map data store in a dim 2 char.
  // std::vector<std::vector<char>> map;
  // single instance
  Data() = default;

public:
  Data(Data const &) = delete;

  void operator=(Data const &) = delete;

  static void parse(const std::string &dir, const std::string &file,
                    Json::Value &doc);

  static Data &getInstance() {
    static Data instance;
    return instance;
  }
};
#endif
