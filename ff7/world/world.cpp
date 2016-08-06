#define libff7_source
#include "ff7/world/world.h"
#include "ff7/flevel/script/string.h"
#include "util/copy.h"

using std::uint16_t;



std::vector<std::vector<char>> ff7::world::split_mes(util::array_view data)
{
  auto pos      = util::make_checked(data);
  auto nstrings = util::get<uint16_t>(pos);

  std::vector<uint16_t> offsets(nstrings);
  util::read(pos, offsets);

  std::vector<std::vector<char>> out;

  for (auto i = 0u; i < nstrings; ++i) {
    try {
      auto pos = util::make_checked(data) + offsets[i];
      out.push_back(flevel::script::string::read_string(
        {pos, data.size() - offsets[i]}));
    } catch (std::exception const& e) {
      throw error{"Error in string " + std::to_string(i) + ": " + e.what()};
    }
  }
  return out;
}



std::vector<char> ff7::world::join_mes(std::vector<std::vector<char>> const& data)
{
  std::vector<char> out;
  auto o = std::back_inserter(out);
  auto sz = static_cast<uint16_t>(data.size());
  std::vector<uint16_t> offsets(sz);

  util::write(o, sz, offsets);

  for (auto i = 0u; i < sz; ++i) {
    offsets[i] = static_cast<uint16_t>(out.size());
    util::write(o, data[i]);
  }

  util::write(out.begin(), sz, offsets);

  if (out.size() > 0x1000)
    throw error{"mes exceeds file limit of 0x1000 bytes"};

  out.resize(0x1000);
  return out;
}

