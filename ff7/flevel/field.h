#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <stdexcept>
#include <string>

namespace ff7 {
namespace flevel {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



class libff7 Field
{
public:
  enum class Section
  {
    script, camera, models, palette, walkmesh,
    tilemap, encounter, triggers, background
  };

  Field(util::array_view data);
  std::vector<char>& operator[](Section sec);
  std::vector<char> const& operator[](Section sec) const;
  std::vector<char> write() const;


private:
  std::array<std::vector<char>, 9> sections;
};



libff7 bool is_field(std::string const& name);

} // namespace flevel
} // namespace ff7

