#include "touphScript/kernel/kernel2.h"
#include "touphScript/string.h"
#include "ff7/kernel/kernel2.h"
#include "util/copy.h"
#include "util/filesystem.h"
#include "util/xml.h"
#include <fstream>
#include <unordered_map>

namespace rxml = rapidxml;

namespace {

namespace xml = util::xml;
using ff7::kernel2::Function;
using std::uint8_t;

std::array<std::string, 18> const section_names{
"command window descriptions",
"ability descriptions",
"item descriptions",
"weapon descriptions",
"armor descriptions",
"accessory descriptions",
"materia descriptions",
"key item descriptions",
"commands",
"abilities",
"items",
"weapons",
"armor",
"accessories",
"materia",
"key items",
"misc",
"summon attacks"
};

std::array<std::string, 7> const functions{
"char", "item", "num", "target", "attack", "id", "element",
};



std::string to_xml(util::array_view data)
{
  std::string out;

  for (auto c = util::make_checked(data), end = c + data.size(); c != end; ++c)
    switch (static_cast<uint8_t>(*c)) {
      case Function::character: case Function::item:   case Function::number:
      case Function::target:    case Function::attack: case Function::id:
      case Function::element:
        out += xml::elem(functions[static_cast<uint8_t>(*c) - Function::character]);
        c += 2;
        break;

      case Function::red:
        out += xml::elem("red");
        ++c;
        break;

      default:
        out += touphScript::string::getc(*c);
        break; 
    }
  return out;
}



void read_function(rxml::xml_node<> const* n, std::vector<char>& out)
try {
  static auto const func_lookup = [] {
    std::unordered_map<std::string, uint8_t> map;
    for (uint8_t i = Function::character; i <= Function::element; ++i)
      map[functions[i - Function::character]] = i;
    map["red"] = Function::red;
    return map;
  }();

  switch (auto c = func_lookup.at(n->name())) {
    case Function::character: case Function::item:   case Function::number:
    case Function::target:    case Function::attack: case Function::id:
    case Function::element:
      out.insert(out.end(), {static_cast<char>(c),
        static_cast<char>(0xFF), static_cast<char>(0xFF)});
      break;

    case Function::red:
      out.insert(out.end(), {static_cast<char>(c), 0x02});
      break;
  }

} catch (std::exception const&) {
  throw touphScript::kernel2::error{
    "Invalid function at char " + std::to_string(out.size())
  };
}



std::vector<char> from_xml(rxml::xml_node<> const* n)
{
  std::vector<char> out;

  for (; n; n = n->next_sibling()) {
    if (n->type() == rxml::node_element)
      read_function(n, out);
    else if (n->type() == rxml::node_data)
      touphScript::string::read_text(n, out);
  }

  out.push_back(static_cast<char>(0xFF));
  return out;
}

} // namespace



namespace fs = boost::filesystem;



void touphScript::kernel2::dump(std::string const& kernel2,
  std::string const& textPath)
{
  std::ifstream file{kernel2, file.binary};
  std::vector<char> data{
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };

  auto sections = ff7::kernel2::split(data);

  std::ofstream out((fs::path{textPath}/"kernel2.bin.xml").string(), out.binary);

  out << "<!DOCTYPE kernel2 SYSTEM \"ts_kernel2.dtd\">\n\n"
      << "<kernel2>\n\n";

  for (auto i = 0u; i < sections.size(); ++i) {
    out << "<section type=\"" << section_names[i] << "\">\n\n";
    for (auto s = 0u; s < sections[i].size(); ++s)
      out << "<string id=\"" << s << "\">" << to_xml(sections[i][s])
          << "</string>\n";
    out << "\n</section>\n\n";
  }

  out << "</kernel2>\n\n";
}



void touphScript::kernel2::encode(std::string const& kernel2,
  std::string const& textPath)
{

  std::ifstream xml_file{(fs::path{textPath}/"kernel2.bin.xml").string(), xml_file.binary};
  std::string xml{
    std::istreambuf_iterator<char>{xml_file}, 
    std::istreambuf_iterator<char>{}
  };

  rxml::xml_document<> doc;
  doc.parse<rxml::parse_default>(&xml[0]);

  auto root = doc.first_node("kernel2");
  if (!root)
    throw error{"Root node kernel2 not found"};

  auto section = root->first_node("section");

  ff7::kernel2::Strings sections;

  for (auto i = 0u; i < sections.size();
    section = section->next_sibling("section"), ++i) {
    if (!section)
      throw error{"Insufficient section tags in kernel2"};

    for (auto string = section->first_node("string"); string;
      string = string->next_sibling("string"))
      sections[i].push_back(from_xml(string->first_node()));
  }

  auto data = ff7::kernel2::join(sections);

  std::ofstream{kernel2, std::ios::binary}.write(data.data(), data.size());
}

