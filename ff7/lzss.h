#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>

namespace ff7 {
namespace lzss { 

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



enum class Compression { size = 4096, balanced = 64, speed = 2 };

libff7 std::vector<char> decompress(util::array_view data);
libff7 std::vector<char>
compress(util::array_view data, Compression comp = Compression::size);

} // namespace lzss
} // namespace ff7

