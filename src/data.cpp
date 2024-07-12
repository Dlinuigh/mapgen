#include "data.h"
#include <fstream>
void Data::parse(std::string dir, std::string file, Json::Value &doc) {
  std::ifstream ifs((dir + file).c_str(), std::ifstream::binary);
  ifs >> doc;
}