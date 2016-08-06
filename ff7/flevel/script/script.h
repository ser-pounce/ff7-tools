#pragma once
#include "ff7/config.h"
#include "util/array_view.h"

namespace ff7 {
namespace flevel {
namespace script {

struct error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



struct libff7 Script
{
  using Scripts = std::array<std::vector<char>, 32>;
  using String  = std::array<char, 8>;
  using Object  = std::pair<String, Scripts>;

  Script(util::array_view data);
  std::vector<char> write() const;

  uint8_t nModels;
  std::uint16_t scale;
  String creator;
  String name;
  std::vector<Object> objects;
  std::vector<std::vector<char>> text;
  std::vector<std::vector<char>> extra;
};

} // script
} // flevel
} // ff7

