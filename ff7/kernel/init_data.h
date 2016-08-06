#pragma once
#include <array>
#include <cstdint>

namespace ff7 {
namespace kernel {

#pragma pack (push, 1)
struct Init
{
  struct Char_record
  {
    uint8_t  id, level,
             strength, vitality, magic, spirit, dexterity, luck,
             strength_b, vitality_b, magic_b, spirit_b, dexterity_b, luck_b,
             limit_level, limit_bar;
    std::array<char, 12> name;
    uint8_t  weapon, armor, accessory,
             flags,
             order,
             lv_progress;
    uint16_t skills,
             nKills;
    std::array<uint16_t, 3> limit_use;
    uint16_t hp, base_hp,
             mp, base_mp,
             unknown0[2],
             maxhp, maxmp;
    uint32_t exp;
    std::array<uint32_t, 8> weapon_materia;
    std::array<uint32_t, 8> armor_materia;
    uint32_t next_level;
  };

  std::array<Char_record, 9> characters;

  std::array<uint8_t, 3>  party;
  std::array<uint16_t, 320> items;
  std::array<uint32_t, 200> materia;
  std::array<uint32_t, 48> yuffie_materia;
  uint32_t unknown0[4],
           gil,
           seconds,
           timer,
           unknown1[2];
};
#pragma pack (pop)

} // namespace kernel
} // namespace ff7

