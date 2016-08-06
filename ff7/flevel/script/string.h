#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>

namespace ff7 {
namespace flevel {
namespace script {
namespace string {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



enum Char : std::uint8_t
{
  amp     = 0x06,
  lt      = 0x1C,
  gt      = 0x1E,
  ltab    = 0xE0,
  new1    = 0xE8,
  new2    = 0xE9,
  cloud   = 0xEA,
  barret  = 0xEB,
  tifa    = 0xEC,
  aerith  = 0xED,
  redxiii = 0xEE,
  yuffie  = 0xEF,
  cait    = 0xF0,
  vincent = 0xF1,
  cid     = 0xF2,
  party1  = 0xF3,
  party2  = 0xF4,
  party3  = 0xF5,
  func    = 0xFE,
  end     = 0xFF,
};



enum Function : std::uint8_t
{
  gray    = 0xD2,
  blue    = 0xD3,
  red     = 0xD4,
  purple  = 0xD5,
  green   = 0xD6,
  cyan    = 0xD7,
  yellow  = 0xD8,
  white   = 0xD9,
  flash   = 0xDA,
  rainbow = 0xDB,
  ok      = 0xDC,
  pause   = 0xDD,
  var1    = 0xDE,
  var2    = 0xDF,
  var3    = 0xE1,
  var4    = 0xE2,
  max     = 0xE9,
};



libff7 std::vector<std::vector<char>> read_table(util::array_view data);

libff7 std::vector<char> read_string(util::array_view data);

} // namespace string
} // namespace script
} // namespace flevel
} // namespace ff7

