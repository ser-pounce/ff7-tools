#define libff7_source
#include "ff7/battle/opcodes.h"

int ff7::battle::op_size(Opcode op)
{
  switch (op) {
    case push_v_1:
    case push_v_8:
    case push_v_16:
    case push_v_24:

    case push_a_1:
    case push_a_8:
    case push_a_16:
    case push_a_24:
      return 3;

    case add:
    case sub:
    case mul:
    case div:
    case mod:
    case b_and:
    case b_or:
    case b_not:

    case ifeq:
    case ifneq:
    case ifge:
    case ifle:
    case ifgt:
    case iflt:

    case l_and:
    case l_or:
    case l_not:
      return 1;

    case push_8:
      return 2;

    case push_16:
      return 3;

    case push_24:
      return 4;

    case jz:
    case jne:
    case jmp:
      return 3;

    case end:
    case zpop:
    case link:
      return 1;

    case mask:
    case rand:
    case rand_bit:
    case any:
    case max:
    case min:
    case mp_cost:
    case set_bit:

    case store:
    case pop:
    case attack:
    case string:
    case mask_stat:
    case store_var:
    case elem_def:

    case debug_str:
    case pop_2:
      return 1;

    default:
      throw error{"Invalid opcode"};
  }
}
