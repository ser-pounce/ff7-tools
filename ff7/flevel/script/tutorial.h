#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>

namespace ff7 {
namespace flevel {
namespace script {
namespace tutorial {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error; 
};



enum Op : std::uint8_t
{
	pause   = 0,
  up      = 0x02,
  down    = 0x03,
  left    = 0x04,
  right   = 0x05,
  menu    = 0x06,
  cancel  = 0x07,
  confirm = 0x09,
	pgup    = 0x0A,
  pgdown  = 0x0C,
  text    = 0x10,
  end     = 0x11,
  window  = 0x12,
  nop     = 0xFF
};




libff7 bool is_tutorial(std::string const& name);

libff7 std::vector<std::vector<char>> split(util::array_view data);

libff7 std::vector<char> join(std::vector<std::vector<char>> const& data);

} // namespace tutorial
} // namespace script
} // namespace flevel
} // namespace ff7

