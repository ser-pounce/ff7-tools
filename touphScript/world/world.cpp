#include "touphScript/world/world.h"
#include "touphScript/string.h"
#include "ff7/lgp.h"
#include "ff7/world/world.h"
#include "ff7/flevel/script/string.h"
#include "util/filesystem.h"
#include "util/copy.h"
#include "util/xml.h"
#include <fstream>
#include <unordered_map>
#include <rapidxml/rapidxml.hpp>

namespace fs     = boost::filesystem;
namespace ts_str = touphScript::string;
namespace xml    = util::xml;
namespace rxml   = rapidxml;
using ff7::flevel::script::string::Char;
using ff7::flevel::script::string::Function;



namespace {

using touphScript::world::error;

std::array<std::string, 10> const colors{
"gray", "blue", "red", "purple", "green", "cyan", "yellow", "white", "flash",
"rainbow"
};



std::string to_xml(std::vector<char> const& data)
{
  std::string str{"<p>"};

  for (auto pos = util::make_checked(data), end = pos + data.size(); pos != end;)
    switch (static_cast<uint8_t>(*pos++)) {
      case Char::func:
        switch (static_cast<uint8_t>(*pos++)) {
          case Function::gray:    case Function::blue:  case Function::red:  
          case Function::purple:  case Function::green: case Function::cyan:
          case Function::yellow:  case Function::white: case Function::flash: 
          case Function::rainbow:
            str += xml::elem(colors[static_cast<uint8_t>(pos[-1]) - Function::gray]);
            break;

          default:
            throw error{"Invalid function near " + str};
        }
        break;

      case Char::ltab:
        str += xml::elem("ltab");
        break;

      case Char::new1: case Char::new2:
        str += "</p>\n<p>";
        break;

      case Char::cloud:   case Char::barret: case Char::tifa: case Char::aerith:
      case Char::redxiii: case Char::yuffie: case Char::cait:
      case Char::vincent: case Char::cid:
        str += xml::elem(ts_str::getc(static_cast<uint8_t>(pos[-1])));
        break; 

      default:
        str += ts_str::getc(static_cast<uint8_t>(pos[-1]));
        break;
    }

  return str + "</p>";
}



void read_tag(rxml::xml_node<> const* n, std::vector<char>& out)
{
  using namespace std::string_literals;

  static auto const func_lookup = [] {
    std::unordered_map<std::string, uint8_t> map;
    map[ts_str::getc(Char::ltab)] = Char::ltab;
    for (uint8_t i = Char::cloud; i <= Char::cid; ++i)
      map[ts_str::getc(i)] = i;
    for (uint8_t i = Function::gray; i <= Function::rainbow; ++i)
      map[colors[i - Function::gray]] = i;
    return map;
  }();

  auto o = std::back_inserter(out);

  auto f = func_lookup.find(n->name());
  if (f == func_lookup.end())
    throw error{"Unrecognized function: " + f->first};

  switch (auto c = f->second) {
    case Function::gray:    case Function::blue:  case Function::red:  
    case Function::purple:  case Function::green: case Function::cyan:
    case Function::yellow:  case Function::white: case Function::flash:
    case Function::rainbow:
      util::write(o, Char::func, c);
      break;

    case Char::cloud:   case Char::barret: case Char::tifa: case Char::aerith:
    case Char::redxiii: case Char::yuffie: case Char::cait: case Char::vincent:
    case Char::cid:     case Char::ltab:
      out.push_back(c);
      break;

    default:
      throw error{"Invalid tag: "s + n->name()}; 
  }
}



std::vector<char> read_xml_string(rxml::xml_node<> const* str)
{
  std::vector<char> out;
  auto lines = 1u;

  try {
    for (auto p = str->first_node("p"); p; p = p->next_sibling("p")) {
      for (auto n = p->first_node(); n; n = n->next_sibling()) {
        if (n->type() == rxml::node_data)
          ts_str::read_text(n, out);
        else if (n->type() == rxml::node_element)
          read_tag(n, out);
      }
      out.push_back(static_cast<char>(Char::new1));
      ++lines;
    }

    out.pop_back();
    out.push_back(static_cast<char>(Char::end));
    return out;

  } catch (std::exception const& e) {
    throw error{"line " + std::to_string(lines) + ": " + e.what()};
  }
}



std::vector<std::vector<char>> from_xml(std::string data)
{
  if (data.empty())
    return {};

  rxml::xml_document<> doc;
  doc.parse<rxml::parse_default>(&data[0]);

  std::vector<std::vector<char>> out;

  auto root = doc.first_node("world");
  if (!root)
    throw error{"Root node world not found"};

  for (auto str = root->first_node("string");
    str; str = str->next_sibling("string")) {

    auto id = xml::ul(xml::att(str, "id"));

    if (out.size() < id + 1)
      out.resize(id + 1);

    try {
      out[id] = read_xml_string(str);
    } catch (std::exception const& e) {
      throw error{
        "Error in string " + std::to_string(id) + ": " + e.what()};
    }
  }
  return out;
}

} // namespace 



void touphScript::world::dump(std::string const& world,
  std::string const& textPath)
{
  std::ofstream out((fs::path{textPath}/"mes.xml").string(), out.binary);
  auto strings = ff7::world::split_mes(ff7::lgp::Archive{world}["mes"]->data());

  out << "<!DOCTYPE world SYSTEM \"ts_world.dtd\">\n\n"
      << "<world>\n\n";
  
  for (auto i = 0u; i < strings.size(); ++i)
    try {
      out << "<string" << util::xml::att("id", i) << ">\n"
          << to_xml(strings[i]) << "\n</string>\n\n";
    } catch (std::exception const& e) {
      throw error{"Error in string " + std::to_string(i) + ": " + e.what()};
    }

  out << "</world>\n";
}



void touphScript::world::encode(std::string const& world,
  std::string const& textPath)
{
  std::ifstream str{(fs::path{textPath}/"mes.xml").string(), str.binary};

  std::string xml_data{
    std::istreambuf_iterator<char>{str}, 
    std::istreambuf_iterator<char>{}
  };

  ff7::lgp::Archive lgp{world};
  lgp["mes"] = ff7::lgp::make_file(
    ff7::world::join_mes(from_xml(std::move(xml_data))));

  auto temp = fs::temp_directory_path() / "temp.lgp";
  lgp.write(temp.string());
  lgp.clear();

  copy_file(temp, world, fs::copy_option::overwrite_if_exists);
  remove(temp);
}

