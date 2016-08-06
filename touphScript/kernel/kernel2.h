#pragma once
#include <stdexcept>
#include <string>

namespace touphScript {
namespace kernel2 {

struct error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

void dump(std::string const& kernel, std::string const& textPath);
void encode(std::string const& kernel, std::string const& textPath);

} // namespace kernel2
} // namespace touphScript

