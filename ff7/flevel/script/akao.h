#pragma once
#include "util/copy.h"

namespace ff7 {
namespace flevel {
namespace script {
namespace akao {

inline bool valid(std::vector<char> const& data)
{
#pragma pack(push, 1)
  struct
  {
    std::array<char, 4> magic;
    std::uint16_t id;
    std::uint16_t len;
    std::uint8_t u[8];
  } header;
#pragma pack(pop)

  if (data.size() < sizeof header)
    return false;

  util::read(data.begin(), header);

  return header.magic == decltype(header.magic){'A','K','A','O'};
}

} // namespace akao
} // namespace script
} // namespace flevel
} // namespace ff7

