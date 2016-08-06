#include <stdexcept>
#include <string>
#include <vector>
#include <rapidxml/rapidxml.hpp>

namespace touphScript {
namespace string {

class error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



void read_text(rapidxml::xml_node<> const* n, std::vector<char>& out);

std::string getc(uint8_t c);

} // namespace string
} // namespace touphScript

