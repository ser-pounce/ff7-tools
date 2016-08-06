#pragma once
#include <stdexcept>
#include <string>
#include <vector>

namespace touphScript {
namespace flevel {
namespace tutorial {

struct error final : public std::runtime_error
{
  using std::runtime_error::runtime_error; 
};

void dump(std::string const& file, std::vector<char> const& data);
std::vector<char> encode(std::string const& file);

} // namespace tutorial
} // namespace flevel
} // namespace touphScript

