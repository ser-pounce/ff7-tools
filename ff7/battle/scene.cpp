#define libff7_source
#include "ff7/battle/scene.h"
#include "ff7/gzip.h"
#include "util/copy.h"
#include <iostream>

namespace {


ff7::battle::Scene read_scene(util::array_view data)
{
  ff7::battle::Scene scene;
  auto unc = ff7::gzip::decompress(data, sizeof scene);
  std::copy(unc.data(), unc.data() + unc.size(),
    reinterpret_cast<char*>(&scene));
  return scene;
}



void read_section(util::array_view data, std::array<ff7::battle::Scene, 256>& scenes, unsigned& scene)
{
  std::array<uint32_t, 16> offsets;
  util::read(util::make_checked(data), offsets);

  for (auto off : offsets)
    if (off != 0xFFFF'FFFF) {
      off *= sizeof off;
      scenes.at(scene++) = read_scene(
        {util::make_checked(data) + off, data.size() - off}
      );
    }
}



std::vector<std::array<char, 0x2000>> split_sections(util::array_view data)
{
  std::vector<std::array<char, 0x2000>> sections(data.size() / 0x2000);
  auto pos = util::make_checked(data);

  for (auto& section : sections)
    std::copy(pos, pos + section.size(), section.data());

  return sections;
}

} // namespace



std::array<ff7::battle::Scene, 256> ff7::battle::split(util::array_view data)
{
  std::array<ff7::battle::Scene, 256> scenes;
  auto scene = 0u;

  for (auto& section : split_sections(data))
    read_section(section, scenes, scene);

  return scenes;
}

