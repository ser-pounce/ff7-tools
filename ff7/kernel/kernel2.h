#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>

namespace ff7 {
namespace kernel2 {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};


enum Function : std::uint8_t
{
  character = 0xEA,
  item      = 0xEB,
  number    = 0xEC,
  target    = 0xED,
  attack    = 0xEE,
  id        = 0xEF,
  element   = 0xF0,
  red       = 0xF8
};

using Strings = std::array<std::vector<std::vector<char>>, 18>;

libff7 Strings split(util::array_view data);
libff7 std::vector<char> join(Strings const& data);

} // namespace kernel2
} // namespace ff7

