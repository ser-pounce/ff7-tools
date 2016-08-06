#include "touphScript/flevel/tutorial.h"
#include "ff7/flevel/script/tutorial.h"
#include "touphScript/string.h"
#include "util/array_view.h"
#include "util/copy.h"
#include "util/xml.h"
#include <fstream>
#include <unordered_map>

namespace {


namespace ts_str = touphScript::string;
namespace xml    = util::xml;
namespace rxml   = rapidxml;
using ff7::flevel::script::tutorial::Op;
using touphScript::flevel::tutorial::error;
using std::uint8_t;
using std::uint16_t;



std::array<std::string, 19> const op_names{
"pause", "", "up", "down", "left", "right", "menu", "cancel",
"", "confirm", "pgup", "", "pgdown", "", "", "", "text", "end", "window"
};



std::string dump_tutorial(util::array_view data)
{
  std::string out;

  for (auto pos = util::make_checked(data);
    static_cast<Op>(*pos) != Op::end; out += '\n') {
 
    switch (static_cast<Op>(*pos)) {

      case Op::pause:
        out += xml::elem("pause", xml::att("frames", util::get<uint16_t>(++pos)));
        break;

      case Op::text:
        out += "<text>";
        for (pos += 2; static_cast<uint8_t>(*pos) != 0xFF; ++pos)
          out += touphScript::string::getc(static_cast<uint8_t>(*pos));
        ++pos;
        out += "</text>";
        break;

      case Op::window: {
        auto x = util::get<uint16_t>(++pos);
        auto y = util::get<uint16_t>(pos);
        out += xml::elem("window", xml::att("x", x) + xml::att("y", y));
        break;
      }

      case Op::up:      case Op::down: case Op::left:
      case Op::right:   case Op::menu: case Op::cancel:
      case Op::confirm: case Op::pgup: case Op::pgdown:
        out += xml::elem(op_names[static_cast<uint8_t>(*pos++)]);
        break;

      case Op::nop:
        out += xml::elem("nop");
        ++pos;
        break;

      default:
        throw error{"Unrecognized tutorial op " +
          std::to_string(static_cast<uint8_t>(*pos)) +
          " at byte " + std::to_string(pos - util::make_checked(data))};
    }
  }

  return out;
}



std::vector<char> from_xml(rxml::xml_node<> const* root)
{
  using namespace std::string_literals;

  static auto const tag_map = [] {
    std::unordered_map<std::string, uint8_t> map;
    for (uint8_t i{}; i < op_names.size(); ++i)
      map[op_names[i]] = i;
    map["nop"] = Op::nop;
    return map;
  }();

  std::vector<char> out;
  auto o = std::back_inserter(out);

  int line{};

  for (auto n = root->first_node(); n; n = n->next_sibling(), ++line) {

    if (n->type() == rxml::node_data)
      continue;

    auto t = tag_map.find(n->name());
    if (t == tag_map.end())
      throw error{"Unrecognized tag: "s + n->name()};

    try {
      switch (t->second) {
        case Op::pause:
          util::write(o, Op::pause, xml::u<uint16_t>(xml::att(n, "frames")));
          break;

        case Op::text:
          util::write(o, Op::text, uint8_t{});
          touphScript::string::read_text(n->first_node(), out);
          o = static_cast<char>(0xFF);
          break;

        case Op::window:
          util::write(o, Op::window, xml::u<uint16_t>(xml::att(n, "x")),
            xml::u<uint16_t>(xml::att(n, "y")));
          break;

        case Op::up:      case Op::down: case Op::left:
        case Op::right:   case Op::menu: case Op::cancel:
        case Op::confirm: case Op::pgup: case Op::pgdown:
        case Op::nop:
          util::write(o, t->second);
          break;
      }
    } catch (std::exception const& e) {
      throw error{"Error on line " + std::to_string(line) + ": " + e.what()};
    }
  }

  o = Op::end;
  return out;
}

} // namespace



void touphScript::flevel::tutorial::dump(std::string const& file,
  std::vector<char> const& data)
try {
  std::ofstream out(file, out.binary);

  out << "<!DOCTYPE field SYSTEM \"ts_tutorial.dtd\">\n\n"
      << "<field>\n\n";

  auto tuts = ff7::flevel::script::tutorial::split(data);

  for (auto t = 0u; t < tuts.size(); ++t)
    try {
      out << "<tutorial" << util::xml::att("id", t) << ">\n\n"
          << dump_tutorial(tuts[t]) << "\n</tutorial>\n\n";
    } catch (std::exception const& e) {
      throw error{"tutorial " + std::to_string(t) + ": " + e.what()};
    }

   out << "</field>\n\n";

} catch (std::exception const& e) {
  throw error{file + ": " + e.what()};
}



std::vector<char> touphScript::flevel::tutorial::encode(std::string const& file)
{
  std::ifstream str{file, str.binary};
  std::string data{
    std::istreambuf_iterator<char>{str}, 
    std::istreambuf_iterator<char>{}
  };

  if (data.empty())
    return {};

  std::vector<std::vector<char>> out;

  rxml::xml_document<> doc;
  doc.parse<rxml::parse_default>(&data[0]);

  auto root = doc.first_node("field");
  if (!root)
    throw error{"Root node field not found"};

  for (auto n = root->first_node("tutorial"); n;
    n = n->next_sibling("tutorial")) {

    auto id = util::xml::ul(util::xml::att(n, "id"));

    if (out.size() < id + 1)
      out.resize(id + 1);

    out[id] = from_xml(n);
  }

  return ff7::flevel::script::tutorial::join(out);
}

