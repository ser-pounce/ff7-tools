#define libff7_source
#include "ff7/flevel/script/string.h"
#include "util/copy.h"

using std::uint8_t;
using std::uint16_t;



std::vector<char> ff7::flevel::script::string::read_string(util::array_view data)
try {
  std::vector<char> str;

  auto it = make_checked(data);

  while (true) {

    str.push_back(*it++);

    if (static_cast<Char>(str.back()) == Char::end)
      break;

    if (static_cast<Char>(str.back()) == Char::func) {

      str.push_back(*it++);
      
      auto sz = 0u;
      
      switch (static_cast<Function>(str.back())) {
        case Function::pause:
          sz = sizeof(uint16_t);
          break;

        case Function::var4:
          sz = sizeof(uint16_t) * 2;
          break;

        case Function::gray:    case Function::blue:  case Function::red:  
        case Function::purple:  case Function::green: case Function::cyan:
        case Function::yellow:  case Function::white: case Function::flash:
        case Function::rainbow: case Function::ok:    case Function::var1:  
        case Function::var2:    case Function::var3:  case Function::max:
          break;

        default:
          throw error{"Invalid function at byte " + std::to_string(str.size())};
      }

      while (sz--)
        str.push_back(*it++);
    }
  }
  return str;

} catch (std::out_of_range const&) {
  throw error{"Unexpected end of string"};
}



std::vector<std::vector<char>>
ff7::flevel::script::string::read_table(util::array_view data)
{
  auto it = util::make_checked(data);
  auto nStrings = util::get<uint16_t>(it + sizeof(uint16_t))
    / sizeof(uint16_t) - 1;

  std::vector<uint16_t> strOffsets(nStrings);
  util::read(it + sizeof(uint16_t), strOffsets);

  std::vector<std::vector<char>> text;

  for (auto str = 0u; str < nStrings; ++str)
    try {
      text.push_back(read_string({it + strOffsets[str], data.size()}));
    } catch (error const& e) {
      throw error{"Error parsing string " +
        std::to_string(str) + ": " + e.what()};
    }

  return text;
}

