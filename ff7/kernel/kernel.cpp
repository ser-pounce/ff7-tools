#define libff7_source
#include "ff7/kernel/kernel.h"
#include "ff7/gzip.h"
#include "util/copy.h"

using std::uint16_t;



std::array<std::vector<char>, 27> ff7::kernel::split(util::array_view data)
{
  std::array<std::vector<char>, 27> out;

  auto pos = util::make_checked(data);

  for (auto& o : out) {
    std::vector<char> cmp(util::get<uint16_t>(pos));
    auto unc = util::get<uint16_t>(pos);
    pos += sizeof(uint16_t);
    util::read(pos, cmp);
    o = gzip::decompress(cmp, unc);
  }

  return out;
}



std::vector<char>
ff7::kernel::join(std::array<std::vector<char>, 27> const& data)
{
  std::vector<char> out;
  auto o = std::back_inserter(out);

  for (uint16_t i{}; i < data.size(); ++i) {
    auto cmp = gzip::compress(data[i]);
    util::write(o,
      static_cast<uint16_t>(cmp.size()),
      static_cast<uint16_t>(data[i].size()),
      i, cmp
    );
  }

  return out;
}

