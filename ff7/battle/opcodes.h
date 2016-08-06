#pragma once
#include "error.h"
#include <stdexcept>

namespace ff7 {
namespace battle {



enum Opcode : std::uint8_t
{
  push_v_1  = 0x00,
  push_v_8  = 0x01,
  push_v_16 = 0x02,
  push_v_24 = 0x03,

  push_a_1  = 0x10,
  push_a_8  = 0x11,
  push_a_16 = 0x12,
  push_a_24 = 0x13,

  add       = 0x30,
  sub       = 0x31,
  mul       = 0x32,
  div       = 0x33,
  mod       = 0x34,
  b_and     = 0x35,
  b_or      = 0x36,
  b_not     = 0x37,

  ifeq      = 0x40,
  ifneq     = 0x41,
  ifge      = 0x42,
  ifle      = 0x43,
  ifgt      = 0x44,
  iflt      = 0x45,

  l_and     = 0x50,
  l_or      = 0x51,
  l_not     = 0x52,

  push_8    = 0x60,
  push_16   = 0x61,
  push_24   = 0x62,

  jz        = 0x70,
  jne       = 0x71,
  jmp       = 0x72,
  end       = 0x73,
  zpop      = 0x74,
  link      = 0x75,

  mask      = 0x80,
  rand      = 0x81,
  rand_bit  = 0x82,
  any       = 0x83,
  max       = 0x84,
  min       = 0x85,
  mp_cost   = 0x86,
  set_bit   = 0x87,

  store     = 0x90,
  pop       = 0x91,
  attack    = 0x92,
  string    = 0x93,
  mask_stat = 0x94,
  store_var = 0x95,
  elem_def  = 0x96,

  debug_str = 0xA0,
  pop_2     = 0xA1,
};

libff7 int op_size(Opcode op);

} // namespace battle
} // namespace ff7

