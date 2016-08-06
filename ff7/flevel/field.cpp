#define libff7_source
#include "ff7/flevel/field.h"
#include "util/copy.h"

namespace {

using std::uint16_t;
using std::uint32_t;



#pragma pack(push, 1)
struct Header
{
  uint16_t null1{};
  uint32_t nBlocks{static_cast<uint32_t>(offsets.size())};
  std::array<uint32_t, 9> offsets{}; 
};
#pragma pack(pop)

} // namespace



ff7::flevel::Field::Field(util::array_view data) try : sections{}
{
  Header h;
  util::read(util::make_checked(data), h);

  if (h.nBlocks != h.offsets.size())
    throw error{"Invalid block count " + std::to_string(h.nBlocks)};

  for (auto i = 0u; i < sections.size(); ++i) {
    if (h.offsets[i]) {
      auto it = util::make_checked(data) + h.offsets[i];
      sections[i].resize(util::get<uint32_t>(it));
      util::read(it, sections[i]);
    }
  }
} catch (std::out_of_range const&) {
  throw error{"Unexpected end of field"};
}



std::vector<char> const& ff7::flevel::Field::operator[](Section sec) const
{
  assert(static_cast<unsigned>(sec) < sections.size());
  return sections[static_cast<int>(sec)];
}



std::vector<char>& ff7::flevel::Field::operator[](Section sec)
{
  return const_cast<std::vector<char>&>(const_cast<Field const&>(*this)[sec]);
}



std::vector<char> ff7::flevel::Field::write() const
{
  std::vector<char> out;
  auto it = std::back_inserter(out);
  Header h;
  util::write(it, h);

  for (auto i = 0u; i < sections.size(); ++i) {
    h.offsets[i] = out.size();
    util::write(it, static_cast<uint32_t>(sections[i].size()), sections[i]);
  }

  util::write(it, "FINAL FANTASY7");
  util::write(out.begin(), h);

  return out;
}



bool ff7::flevel::is_field(std::string const& name)
{
  return name != "maplist" && name.find('.') == name.npos;
}

