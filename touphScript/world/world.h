#pragma once
#include <stdexcept>
#include <string>

namespace touphScript {
namespace world {

struct error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

void dump(std::string const& world, std::string const& textPath);
void encode(std::string const& world, std::string const& textPath);

} // namespace wm
} // namespace touphScript

