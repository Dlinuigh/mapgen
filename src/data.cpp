#include "data.h"
#include <fstream>
void Data::parse(const std::string &dir, const std::string &file, Json::Value &doc) {
  std::ifstream ifs((dir + file).c_str(), std::ifstream::binary);
  ifs >> doc;
}