#pragma once
#include "ff7/config.h"
#include <stdexcept>

namespace ff7 {
namespace battle {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

} // namespace battle
} // namespace ff7

