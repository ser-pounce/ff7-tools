#pragma once
#include <stdexcept>
#include <string>
#include <vector>

namespace touphScript {
namespace flevel {
namespace field {

class error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

void dump(std::string const& file, std::vector<char> const& data);

std::vector<char> encode(std::string const& file,
  std::vector<char> const& data);

} // namespace field
} // namespace flevel
} // namespace touphScript

