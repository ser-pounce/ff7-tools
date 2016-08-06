#define libff7_source
#include "ff7/flevel/script/tutorial.h"
#include "util/copy.h"
#include <iostream>

using std::uint16_t;

bool ff7::flevel::script::tutorial::is_tutorial(std::string const& name)
{
  static std::string const tut = ".tut";
  return name.size() > tut.size() && name.substr(name.size() - tut.size()) == tut;
}



std::vector<std::vector<char>>
ff7::flevel::script::tutorial::split(util::array_view data)
try {
  std::vector<uint16_t> offsets(
    util::get<uint16_t>(util::make_checked(data)) / sizeof(uint16_t));

  util::read(util::make_checked(data), offsets);

  std::vector<std::vector<char>> out(offsets.size());
  offsets.push_back(data.size());

  for (auto i = 0u; i < offsets.size() - 1; ++i)
    try {
      out[i].resize(offsets[i + 1] - offsets[i]);
      util::read(util::make_checked(data) + offsets[i], out[i]);
    } catch (std::out_of_range const&) {
      throw error{"Error reading tutorial " + std::to_string(i)};
    }

  return out;

} catch (std::out_of_range const&) {
  throw error{"Error reading tutorials"};
}



std::vector<char>
ff7::flevel::script::tutorial::join(std::vector<std::vector<char>> const& data)
{
  std::vector<char> out;
  auto o = std::back_inserter(out);
  std::vector<uint16_t> offsets(data.size());
  
  util::write(o, offsets);

  for (auto i = 0u; i < data.size(); ++i) {
    offsets[i] = out.size();
    util::write(o, data[i]);
  }

  util::write(out.begin(), offsets);
  
  return out;
}

