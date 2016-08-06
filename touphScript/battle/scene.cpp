#include "touphScript/battle/scene.h"
#include "touphScript/string.h"
#include "ff7/battle/scene.h"
#include "ff7/battle/opcodes.h"
#include "util/array_view.h"
#include "util/copy.h"
#include "util/filesystem.h"
#include <fstream>



namespace {
/*
void to_xml(std::ostream& out, char const (&data)[504], uint16_t const start)
{
  using namespace ff7::battle;

  auto pos = util::make_checked(data, data + sizeof data) + start;
  auto end = start;

  out << "<formation>\n\n";

  while (true) {
    switch (static_cast<uint8_t>(*pos)) {

      case jz: case jne: case jmp: {
        auto jump = util::get<uint16_t>(++pos);
        if (jump < start)
          throw touphScript::battle::error{"Jump beyond script start"};
        if (jump > end)
          end = jump;
        break;
      }
      case string:
        

    }
  }

  out << "\n</formation>\n\n">;
}
*/

} // namespace



namespace fs = boost::filesystem;

void touphScript::battle::dump(std::string const& scene,
  std::string const& textPath)
{
  std::ifstream file{scene, file.binary};
  std::vector<char> data{
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };
 
  std::ofstream out((fs::path{textPath}/"scene.bin.xml").string(), out.binary);

  out << "<!DOCTYPE scene SYSTEM \"ts_scene.dtd\">\n\n"
      << "<scene>\n\n";

  auto scenes = ff7::battle::split(data);

  for (auto i = 0u; i < scenes.size(); ++i) {
    out << "<file id=\"" << i << "\">\n\n<enemies>\n\n";

    for (auto n = 0u; n < 3; ++n) {

      auto& name = scenes[i].enemy[n].name;

      if (name[0] != static_cast<char>(0xFF)) {
        out << "<string id=\"" << n << "\">";
        for (auto c = name.begin();
        *c != static_cast<char>(0xFF) && c != name.end(); ++c)
          out << string::getc(*c);
        out << "</string>\n";
      }
    }
    
    out << "\n</enemies>\n\n<attacks>\n\n";

    for (auto n = 0u; n < scenes[i].attack_name.size(); ++n) {

      auto& name = scenes[i].attack_name[n];

      if (name[0] != static_cast<char>(0xFF)) {
        out << "<string id=\"" << n << "\">";
        for (auto c = name.begin();
        *c != static_cast<char>(0xFF) && c != name.end(); ++c)
          out << string::getc(*c);
        out << "</string>\n";
      }
    }
    out << "\n</attacks>\n\n</file>\n\n";
  }
  out << "</scene>\n";
}

