#include "touphScript/flevel/field.h"
#include "touphScript/string.h"
#include "ff7/lzss.h"
#include "ff7/flevel/field.h"
#include "ff7/flevel/script/script.h"
#include "ff7/flevel/script/string.h"
#include "util/array_view.h"
#include "util/copy.h"
#include "util/xml.h"
#include <fstream>
#include <unordered_map>


namespace {

namespace ts_str = touphScript::string;
namespace xml    = util::xml;
namespace rxml   = rapidxml;
using ff7::flevel::script::string::Char;
using ff7::flevel::script::string::Function;
using touphScript::flevel::field::error;
using std::uint8_t;
using std::uint16_t;



std::array<std::string, 24> const funcs{
"gray", "blue", "red", "purple", "green", "cyan", "yellow", "white", "flash",
"rainbow", "ok", "pause", "var", "", "", "", "", "", "", "", "", "", "", "max"
};



std::string to_xml(util::array_view data)
try {

  std::string str{"<p>"};

  for (auto it = util::make_checked(data), end = it + data.size(); it != end;) {

    switch (static_cast<uint8_t>(*it++)) {
      case Char::func:
        switch (static_cast<uint8_t>(*it++)) {
          case Function::pause:
            str += xml::elem("pause",
              xml::att("frames", std::to_string(util::get<uint16_t>(it))));
            break;

          case Function::gray:    case Function::blue:  case Function::red:  
          case Function::purple:  case Function::green: case Function::cyan:
          case Function::yellow:  case Function::white: case Function::flash: 
          case Function::rainbow: case Function::ok:    case Function::max:
            str += xml::elem(funcs[static_cast<uint8_t>(it[-1]) - Function::gray]);
            break;

          case Function::var1:
            str += xml::elem("var");
            break;

          case Function::var2:
            str += xml::elem("var", xml::att("format", "hex"));
            break;

          case Function::var3:
            str += xml::elem("var", xml::att("format", "digit"));
            break;

          case Function::var4: {
            auto o = std::to_string(util::get<uint16_t>(it));
            auto l = std::to_string(util::get<uint16_t>(it));
            str += xml::elem("var", xml::att("offset", o) + xml::att("len", l));
            break;
          }

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
      case Char::vincent: case Char::cid:    case Char::party1:
      case Char::party2:  case Char::party3:
        str += xml::elem(ts_str::getc(static_cast<uint8_t>(it[-1])));
        break;

      default:
        str += ts_str::getc(static_cast<uint8_t>(it[-1]));
    }
  }

  return str + "</p>";

} catch (std::out_of_range const&) {
   throw error{"Unexpected end of string"};
}



void read_tag(rxml::xml_node<> const* n, std::vector<char>& out)
{
  using namespace std::string_literals;

  static auto const func_lookup = [] {
    std::unordered_map<std::string, uint8_t> map;
    map[ts_str::getc(Char::ltab)] = Char::ltab;
    for (uint8_t i = Char::cloud; i <= Char::party3; ++i)
      map[ts_str::getc(i)] = i;
    for (uint8_t i = Function::gray; i <= Function::max; ++i)
      map[funcs[i - Function::gray]] = i;
    return map;
  }();

  auto o = std::back_inserter(out);

  auto f = func_lookup.find(n->name());
  if (f == func_lookup.end())
    throw error{"Unrecognized function: " + f->first};

  switch (auto c = f->second) {
    case Function::pause:
      util::write(o, Char::func, c, xml::u<uint16_t>(xml::att(n, "frames")));
      break;

    case Function::gray:    case Function::blue:  case Function::red:  
    case Function::purple:  case Function::green: case Function::cyan:
    case Function::yellow:  case Function::white: case Function::flash:
    case Function::rainbow: case Function::ok:    case Function::max:
      util::write(o, Char::func, c);
      break;

    case Function::var1: {
      auto offset = n->first_attribute("offset");
      auto len    = n->first_attribute("len");

      if ((offset && !len) || (!offset && len))
        throw error{"Must specify both offset and len in var tag"};

      if (offset && len) {
        util::write(o, Char::func, Function::var4,
          xml::u<uint16_t>(offset), xml::u<uint16_t>(len));

      } else if (auto f = n->first_attribute("format")) {
        auto format = f->value();
        util::write(o, Char::func);
        if (format == "hex"s)
          util::write(o, Function::var2);
        else if (format == "digit"s)
          util::write(o, Function::var3);
        else
          throw error{"Invalid var tag format"};
      }

      else if (!n->first_attribute())
        util::write(o, Char::func, Function::var1);
      else
        throw error{"var tag must specify both offset and len,"
          "or format alone, or nothing"};

      break;
    }

    case Char::cloud:   case Char::barret: case Char::tifa: case Char::aerith:
    case Char::redxiii: case Char::yuffie: case Char::cait: case Char::vincent:
    case Char::cid:     case Char::party1: case Char::party2:
    case Char::party3:  case Char::ltab:
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

  auto root = doc.first_node("field");
  if (!root)
    throw error{"Root node field not found"};

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



using namespace ff7::flevel;

void touphScript::flevel::field::dump(std::string const& file,
  std::vector<char> const& data)
try {
  auto field  = Field{ff7::lzss::decompress(data)};
  auto script = script::Script{field[Field::Section::script]};

  std::ofstream out(file, out.binary);

  out << "<!DOCTYPE field SYSTEM \"ts_field.dtd\">\n"
      << "<?xml-stylesheet href=\"style.css\"?>\n\n"
      << "<field>\n\n";

  for (auto t = 0u; t < script.text.size(); ++t)
    try {
      out << "<string" << util::xml::att("id", t) << ">\n"
          << to_xml(script.text[t]) << "\n</string>\n\n";
    } catch (std::exception const& e) {
      throw error{"Error in string " + std::to_string(t) + ": " + e.what()};
    }

  out << "</field>\n\n";

} catch (std::exception const& e) {
  throw error{file + ": " + e.what()};
}



std::vector<char> touphScript::flevel::field::encode(std::string const& file,
  std::vector<char> const& data)
try {
  auto field  = Field{ff7::lzss::decompress(data)};
  auto script = script::Script{field[Field::Section::script]};

  std::ifstream str{file, str.binary};
  std::string xml_data{
    std::istreambuf_iterator<char>{str}, 
    std::istreambuf_iterator<char>{}
  };

  script.text = from_xml(std::move(xml_data));

  field[Field::Section::script] = script.write();

  return ff7::lzss::compress(field.write());

} catch (std::exception const& e) {
  throw error{file + ": " + e.what()};
}

