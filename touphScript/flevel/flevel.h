#pragma once
#include <stdexcept>
#include <string>

namespace touphScript {
namespace flevel {

struct error final : public std::runtime_error
{
  using std::runtime_error::runtime_error; 
};

void dump(std::string const& flevel, std::string const& textPath);
void encode(std::string const& flevel, std::string const& textPath);

} // namespace flevel
} // namespace touphScript

