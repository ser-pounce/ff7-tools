#pragma once
#include "error.h"
#include "util/array_view.h"

namespace ff7 {
namespace battle {



using std::uint8_t;
using std::uint16_t;
using std::uint32_t;


#pragma pack(push, 1)
struct Scene
{
  uint16_t id[3],
           pad0;
  struct
  {
    uint16_t location,
             next_battle_formation,
             escapes,
             pad0,
             next_battle_arena_formation[4],
             escapable;
    uint8_t  layout,
             camera;
  } setup[4];

  struct
  {
    struct Pos
    {
      uint16_t pos_x, pos_y, pos_z,
               dir_x, dir_y, dir_z;
    };
    Pos primary;
    Pos secondary[2];
    uint16_t pad0[6];
  } camera[4];

  struct
  {
    uint16_t id,
             x, y, z,
             row,
             cover,
             condition[2];
  } formation[4];

  struct
  {
    std::array<char, 32> name;
    uint8_t  lv, spd, lck, eva, str, def, mag, mdef,
             element[8],
             element_rates[8],
             animation[16];
    uint16_t attack[16],
             camera[16];
    uint8_t  item_rate[4];
    uint16_t item[4],
             manip_attack[3],
             unknown0,
             mp,
             ap,
             morph;
    uint8_t  back_multi,
             pad0;
    uint32_t hp,
             exp,
             gil,
             status,
             pad1;
  } enemy[3];

  struct
  {
    uint8_t  attack_percent,
             impact_effect,
             hurt,
             unknown0;
    uint16_t cost,
             sound,
             camera_single,
             camera_multi;
    uint8_t  target,
             attack_effect,
             damage,
             str,
             condition,
             status_mod,
             add_effect,
             mod;
    uint32_t status;
    uint16_t element,
             special;
  } attack[32];

  uint16_t attack_id[32];
  std::array<std::array<char, 32>, 32> attack_name;
  uint16_t form_offset[4];
  char form_data[504];
  uint16_t enemy_offset[4];
  char enemy_data[4090];
};
#pragma pack(pop)

libff7 std::array<ff7::battle::Scene, 256> split(util::array_view data);
libff7 std::vector<char> join(std::array<ff7::battle::Scene, 256> const& data);

} // namespace battle
} // namespace ff7

