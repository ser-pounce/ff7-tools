#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>

namespace ff7 {
namespace kernel {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

libff7 std::array<std::vector<char>, 27> split(util::array_view data);
libff7 std::vector<char> join(std::array<std::vector<char>, 27> const& data);

} // namespace kernel
} // namespace ff7

