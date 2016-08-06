#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>

namespace ff7 {
namespace world {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

libff7 std::vector<std::vector<char>> split_mes(util::array_view data);
libff7 std::vector<char> join_mes(std::vector<std::vector<char>> const& data);

} // namespace world
} // namespace ff7

