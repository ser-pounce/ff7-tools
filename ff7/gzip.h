#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>

namespace ff7 {
namespace gzip {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



libff7 std::vector<char> decompress(util::array_view data, std::uint32_t read);

libff7 std::vector<char> compress(util::array_view data);

} // namespace gzip
} // namespace ff7

