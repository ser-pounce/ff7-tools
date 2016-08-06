#define libff7_source
#include "ff7/kernel/kernel2.h"
#include "ff7/lzss.h"
#include "util/copy.h"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;


namespace {

using ff7::kernel2::Function;

std::vector<char> read_string(util::array_view data)
{
  auto c = data.begin();

  while (static_cast<uint8_t>(*c) != 0xFF)
    switch (static_cast<uint8_t>(*c)) {
      case Function::character: case Function::item:   case Function::number:
      case Function::target:    case Function::attack: case Function::id:
      case Function::element:
        c += 3;
        break;

      default:
        ++c;
        break; 
    }
 
  return {data.begin(), c};
}



std::vector<std::vector<char>> read_strings(util::array_view data)
{
  std::vector<uint16_t> offsets(
    util::get<uint16_t>(util::make_checked(data)) / sizeof(uint16_t));
  util::read(util::make_checked(data), offsets);

  std::vector<std::vector<char>> out;

  for (auto& offset : offsets)
    out.push_back(read_string({data.begin() + offset, data.end()}));

  return out;
}



std::vector<char> join_strings(std::vector<std::vector<char>> const& data)
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

} // namespace



ff7::kernel2::Strings ff7::kernel2::split(util::array_view data)
{
  auto unc = lzss::decompress(data);

  Strings strings;
  auto pos = util::make_checked(unc);
  
  for (auto& section : strings) {
    std::vector<char> temp(util::get<uint32_t>(pos));
    util::read(pos, temp);
    section = read_strings(temp);
  }

  return strings;
}



std::vector<char> ff7::kernel2::join(Strings const& data)
{
  std::vector<char> out;
  auto o = std::back_inserter(out);

  for (auto& section : data) {
    auto joined = join_strings(section);
    util::write(o, static_cast<uint32_t>(joined.size()), joined);
  }

  return lzss::compress(out);
}

