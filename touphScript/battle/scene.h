#pragma once
#include <stdexcept>
#include <string>

namespace touphScript {
namespace battle {

struct error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

void dump(std::string const& scene, std::string const& textPath);
void encode(std::string const& scene, std::string const& textPath);

} // namespace battle
} // namespace touphScript

