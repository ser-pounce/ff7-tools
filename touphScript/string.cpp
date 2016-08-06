#include "touphScript/string.h"
#include <array>
#include <unordered_map>

namespace rxml = rapidxml;
using std::uint8_t;

namespace {

std::array<std::string, 256> const chars{
" ", "!", "\"", "#", "$", "%", "&", "\'", "(", ")", "*", "+", ",", "-", ".", "/",
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
"@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
"`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "",
"Ä", "Á", "Ç", "É", "Ñ", "Ö", "Ü", "á", "à", "â", "ä", "ã", "å", "ç", "é", "è",
"ê", "ë", "í", "ì", "î", "ï", "ñ", "ó", "ò", "ô", "ö", "õ", "ú", "ù", "û", "ü",
"♥", "°", "¢", "♠", "Ù", "Û", "¶", "ß", "ū", "©", "™", "´", "¨", "≠", "Æ", "Ø",
"∞", "±", "≤", "≥", "¥", "µ", "∂", "Σ", "Π", "π", "⌡", "ª", "º", "Ú", "æ", "ø",
"¿", "¡", "¬", "√", "ƒ", "≈", "∆", "«", "»", "…", "", "À", "Ã", "Õ", "Œ", "œ",
"–", "—", "“", "”", "‘", "’", "÷", "◊", "ÿ", "Ÿ", "⁄", "¤", "‹", "›", "ﬁ", "ﬂ",
"■", "▪", "‚", "„", "‰", "Â", "Ê", "Ë", "Å", "È", "Í", "Î", "Ï", "Ì", "Ó", "Ô",
"", "Ò", "Ù", "Û", "", "", "", "", "", "", "", "", "", "", "", "",
"ltab", "\t", ", ", ".\"", "…\"", "", "", "\n", "", "", "cloud", "barret", "tifa", "aerith", "redxiii", "yuffie",
"cait", "vincent", "cid", "party1", "party2", "party3", "〇", "△", "☐", "✕", "", "", "", "", "", ""
};



int utf8_len(char const* c, char const* end)
{
  static constexpr std::array<uint8_t, 256> const utf8len{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };

  auto len = utf8len[static_cast<uint8_t>(*c)];
  if (!len || c + len > end)
    throw touphScript::string::error{"Invalid UTF-8 character"};

  return len;
}

} // namespace



void touphScript::string::read_text(rxml::xml_node<> const* n,
  std::vector<char>& out)
{
  static auto const char_lookup = [] {
    std::unordered_map<std::string, uint8_t> map;
    for (auto i = 0u; i < chars.size(); ++i)
      map[chars[i]] = i;
    return map;
  }();

  if (!n)
    return;
  if (n->type() != rxml::node_data)
    throw error{"Node must be text"};

  int chars{};

  for (auto c = n->value(), end = c + n->value_size(); *c;)
    try {
      auto len = utf8_len(c, end);
      out.push_back(char_lookup.at({c, c + len}));
      c += len;
      ++chars;
    } catch (std::exception const&) {
      throw error{"Invalid character at char " + std::to_string(chars)};
    }
}



std::string touphScript::string::getc(uint8_t c)
{
  enum : uint8_t { amp = 0x06, lt = 0x1C, gt = 0x1E };

  switch (c) {
    case amp:
      return "&amp;";

    case lt:
      return "&lt;";

    case gt:
      return "&gt;";

    default:
      return chars[c];  
  }
}

