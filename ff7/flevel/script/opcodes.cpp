#define libff7_source
#include "ff7/flevel/script/opcodes.h"
#include "util/copy.h"
#include <cstddef>
#include <limits>

using namespace std::string_literals;

namespace {

using namespace ff7::flevel::script::opcode;
using It = ff7::flevel::script::opcode::It;
void update_jumps(std::vector<Opcode>& ops, It pos, int size);



enum : uint8_t
{
  Unused = 255, Unknown = Unused,
  Ret = 0,
  Kawaiw = 0,
  Hmpmax1 = 0,
  Hmpmax2 = 0,
  Mhmmx = 0,
  Hmpmax3 = 0,
  Nop = 0,
  Scrcc = 0,
  Scrlw = 0,
  Fadew = 0,
  Animw = 0,
  Ofstw = 0,
  Animb = 0,
  Turnw = 0,
  Movie = 0,
  Gameover = 0,
  Flmat = 0,
  Flitm = 0,
  Rsglb = 0,
  Clitm = 0,
  Special = Unused,
  Kawai = Unused
};



static constexpr std::array<uint8_t, 256> const opcodeSz
{
  Ret,                      sizeof(Opcode::Req),     sizeof(Opcode::Reqsw),    sizeof(Opcode::Reqew),   sizeof(Opcode::Preq),    sizeof(Opcode::Prqsw),  sizeof(Opcode::Prqew),  sizeof(Opcode::Retto),
  sizeof(Opcode::Join),     sizeof(Opcode::Split),   sizeof(Opcode::Sptye),    sizeof(Opcode::Gptye),   Unused,                  Unused,                 sizeof(Opcode::Dskcg),  Special,
  sizeof(Opcode::Jmpf),     sizeof(Opcode::Jmpfl),   sizeof(Opcode::Jmpb),     sizeof(Opcode::Jmpbl),   sizeof(Opcode::Ifub),    sizeof(Opcode::Ifubl),  sizeof(Opcode::Ifsw),   sizeof(Opcode::Ifswl),
  sizeof(Opcode::Ifuw),     sizeof(Opcode::Ifuwl),   Unused,                   Unused,                  Unused,                  Unused,                 Unused,                 Unused,
  sizeof(Opcode::Minigame), sizeof(Opcode::Tutor),   sizeof(Opcode::Btmd2),    sizeof(Opcode::Btrld),   sizeof(Opcode::Wait),    sizeof(Opcode::Nfade),  sizeof(Opcode::Blink),  sizeof(Opcode::Bgmovie),
  Kawai,                    Kawaiw,                  sizeof(Opcode::Pmova),    sizeof(Opcode::Slip),    sizeof(Opcode::Bgpdh),   sizeof(Opcode::Bgscr),  sizeof(Opcode::Wcls),   sizeof(Opcode::Wsizw),
  sizeof(Opcode::Ifkey),    sizeof(Opcode::Ifkeyon), sizeof(Opcode::Ifkeyoff), sizeof(Opcode::Uc),      sizeof(Opcode::Pdira),   sizeof(Opcode::Ptura),  sizeof(Opcode::Wspcl),  sizeof(Opcode::Wnumb),
  sizeof(Opcode::Sttim),    sizeof(Opcode::Goldu),   sizeof(Opcode::Goldd),    sizeof(Opcode::Chgld),   Hmpmax1,                 Hmpmax2,                Mhmmx,                  Hmpmax3,
  sizeof(Opcode::Message),  sizeof(Opcode::Mpara),   sizeof(Opcode::Mpara2),   sizeof(Opcode::Mpnam),   Unused,                  sizeof(Opcode::Mpu),    Unused,                 sizeof(Opcode::Mpd),
  sizeof(Opcode::Ask),      sizeof(Opcode::Menu),    sizeof(Opcode::Menu2),    sizeof(Opcode::Btltb),   Unused,                  sizeof(Opcode::Hpu),    Unused,                 sizeof(Opcode::Hpd),
  sizeof(Opcode::Window),   sizeof(Opcode::Wmove),   sizeof(Opcode::Wmode),    sizeof(Opcode::Wrest),   sizeof(Opcode::Wclse),   sizeof(Opcode::Wrow),   sizeof(Opcode::Gwcol),  sizeof(Opcode::Swcol),
  sizeof(Opcode::Stitm),    sizeof(Opcode::Dlitm),   sizeof(Opcode::Ckitm),    sizeof(Opcode::Smtra),   sizeof(Opcode::Dmtra),   sizeof(Opcode::Cmtra),  sizeof(Opcode::Shake),  Nop,
  sizeof(Opcode::Mapjump),  Unknown,                 sizeof(Opcode::Scrlc),    sizeof(Opcode::Scrla),   sizeof(Opcode::Scr2d),   Scrcc,                  sizeof(Opcode::Scr2dc), Scrlw,
  sizeof(Opcode::Scr2dl),   sizeof(Opcode::Mpdsp),   sizeof(Opcode::Vwoft),    sizeof(Opcode::Fade),    Fadew,                   sizeof(Opcode::Idlck),  sizeof(Opcode::Lstmp),  sizeof(Opcode::Scrlp),
  sizeof(Opcode::Battle),   sizeof(Opcode::Btlon),   sizeof(Opcode::Btlmd),    sizeof(Opcode::Pgtdr),   sizeof(Opcode::Getpc),   sizeof(Opcode::Pxyzi),  sizeof(Opcode::Plusx),  sizeof(Opcode::Plus2x),
  sizeof(Opcode::Minusx),   sizeof(Opcode::Minus2x), sizeof(Opcode::Incx),     sizeof(Opcode::Inc2x),   sizeof(Opcode::Decx),    sizeof(Opcode::Dec2x),  sizeof(Opcode::Tlkon),  sizeof(Opcode::Rdmsd),
  sizeof(Opcode::Setbyte),  sizeof(Opcode::Setword), sizeof(Opcode::Biton),    sizeof(Opcode::Bitoff),  sizeof(Opcode::Bitxor),  sizeof(Opcode::Plus),   sizeof(Opcode::Plus2),  sizeof(Opcode::Minus),
  sizeof(Opcode::Minus2),   sizeof(Opcode::Mul),     sizeof(Opcode::Mul2),     sizeof(Opcode::Div),     sizeof(Opcode::Div2),    sizeof(Opcode::Mod),    sizeof(Opcode::Mod2),   sizeof(Opcode::And),
  sizeof(Opcode::And2),     sizeof(Opcode::Or),      sizeof(Opcode::Or2),      sizeof(Opcode::Xor),     sizeof(Opcode::Xor2),    sizeof(Opcode::Inc),    sizeof(Opcode::Inc2),   sizeof(Opcode::Dec),
  sizeof(Opcode::Dec2),     sizeof(Opcode::Random),  sizeof(Opcode::Lbyte),    sizeof(Opcode::Hbyte),   sizeof(Opcode::Tbyte),   Unknown,                Unknown,                sizeof(Opcode::Searchx),
  sizeof(Opcode::Pc),       sizeof(Opcode::Char),    sizeof(Opcode::Dfanm),    sizeof(Opcode::Anime1),  sizeof(Opcode::Visi),    sizeof(Opcode::Xyzi),   sizeof(Opcode::Xyi),    sizeof(Opcode::Xyz),
  sizeof(Opcode::Move),     sizeof(Opcode::Cmove),   sizeof(Opcode::Mova),     sizeof(Opcode::Tura),    Animw,                   sizeof(Opcode::Fmove),  sizeof(Opcode::Anime2), sizeof(Opcode::Animx1),
  sizeof(Opcode::Canim1),   sizeof(Opcode::Canmx1),  sizeof(Opcode::Msped),    sizeof(Opcode::Dir),     sizeof(Opcode::Turngen), sizeof(Opcode::Turn),   sizeof(Opcode::Dira),   sizeof(Opcode::Getdir),
  sizeof(Opcode::Getaxy),   sizeof(Opcode::Getai),   sizeof(Opcode::Animx2),   sizeof(Opcode::Canim2),  sizeof(Opcode::Canmx2),  sizeof(Opcode::Asped),  Unused,                 sizeof(Opcode::Cc),
  sizeof(Opcode::Jump),     sizeof(Opcode::Axyzi),   sizeof(Opcode::Lader),    sizeof(Opcode::Ofst),    Ofstw,                   sizeof(Opcode::Talkr),  sizeof(Opcode::Slidr),  sizeof(Opcode::Solid),
  sizeof(Opcode::Prtyp),    sizeof(Opcode::Prtym),   sizeof(Opcode::Prtye),    sizeof(Opcode::Ifprtyq), sizeof(Opcode::Ifmembq), sizeof(Opcode::Mmbud),  sizeof(Opcode::Mmblk),  sizeof(Opcode::Mmbuk),
  sizeof(Opcode::Line),     sizeof(Opcode::Linon),   sizeof(Opcode::Mpjpo),    sizeof(Opcode::Sline),   sizeof(Opcode::Sin),     sizeof(Opcode::Cos),    sizeof(Opcode::Tlkr2),  sizeof(Opcode::Sldr2),
  sizeof(Opcode::Pmjmp),    Unknown,                 sizeof(Opcode::Akao2),    sizeof(Opcode::Fcfix),   sizeof(Opcode::Ccanm),   Animb,                  Turnw,                  sizeof(Opcode::Mppal),
  sizeof(Opcode::Bgon),     sizeof(Opcode::Bgoff),   sizeof(Opcode::Bgrol),    sizeof(Opcode::Bgrol2),  sizeof(Opcode::Bgclr),   sizeof(Opcode::Stpal),  sizeof(Opcode::Ldpal),  sizeof(Opcode::Cppal),
  sizeof(Opcode::Rtpal),    sizeof(Opcode::Adpal),   sizeof(Opcode::Mppal2),   sizeof(Opcode::Stpls),   sizeof(Opcode::Ldpls),   sizeof(Opcode::Cppal2), sizeof(Opcode::Rtpal2), sizeof(Opcode::Adpal2),
  sizeof(Opcode::Music),    sizeof(Opcode::Sound),   sizeof(Opcode::Akao),     sizeof(Opcode::Musvt),   sizeof(Opcode::Musvm),   sizeof(Opcode::Mulck),  sizeof(Opcode::Bmusc),  Unknown,
  sizeof(Opcode::Pmvie),    Movie,                   sizeof(Opcode::Mvief),    sizeof(Opcode::Mvcam),   sizeof(Opcode::Fmusc),   sizeof(Opcode::Cmusc),  sizeof(Opcode::Chmst),  Gameover
};



static constexpr std::array<uint8_t, 11> const spOpcodeSz
{
                                                                                                                                 sizeof(Opcode::Arrow),   Unused,                Unused,
  sizeof(Opcode::Smspd),    Flmat,                   Flitm,                    sizeof(Opcode::Btlck),   sizeof(Opcode::Mvlck),   sizeof(Opcode::Spcnm),   Rsglb,                 Clitm
};



bool is_playable(Unpacked const& obj)
{
  for (auto& op : obj[0])
    if (op.op == Opcode::PC)
      return true;
  return false;
}



bool is_model(Unpacked const& obj)
{
  for (auto& op : obj[0])
    if (op.op == Opcode::CHAR)
      return true;
  return false;
}



bool is_line(Unpacked const& obj)
{
  for (auto& op : obj[0])
    if (op.op == Opcode::LINE)
      return true;
  return false;
}



class Basic_stack : public Stack
{
public:
  Basic_stack(Stack const* parent, uint8_t obj, uint8_t script):
    parent_{parent}, obj_{obj}, script_{script} {}

  Basic_stack(Basic_stack const&) =delete;
  Basic_stack& operator=(Basic_stack const&) =delete;

  bool in(uint8_t obj, uint8_t script) const override
  {
    if (obj == obj_ && script == script_)
      return true;
    return parent_ ? parent_->in(obj, script) : false;
  }

  uint8_t obj() const override { return obj_; }
  uint8_t script() const override { return script_; }
  Stack const* parent() const override { return parent_; }

private:
  Stack const* parent_;
  uint8_t obj_, script_;
};



void parse_script(std::vector<Unpacked>& objs, Stack const& stack,
  Scriptwise script_func, Opwise op_func)
{
  if (script_func)
    script_func(stack);

  auto& ops = objs[stack.obj()][stack.script()];

  for (auto op = ops.begin(); op != ops.end(); ++op) {

    if (op->is_obj_call() && !stack.in(op->object(), op->script())) {
      Basic_stack s{&stack, op->object(), op->script()};
      parse_script(objs, s, script_func, op_func);
    }

    if (op->is_party_call())
      for (uint8_t i = 0u; i < objs.size(); ++i)
        if (is_playable(objs[i]) && !stack.in(i, op->script())) {
          Basic_stack s{&stack, i, op->script()};
          parse_script(objs, s, script_func, op_func);
        }

    if (op->is_ret_call() && !stack.in(stack.obj(), op->script())) {
      Basic_stack s{&stack, stack.obj(), op->script()};
      parse_script(objs, s, script_func, op_func);
    }

    if (op_func)
      op_func(op, stack);
  }
}



void jump_set(std::vector<Opcode>& ops, It pos, int val)
{
  static constexpr auto const max = std::numeric_limits<uint8_t>::max();

  if (pos->is_short_jump() && pos->get_short_jump() + val > max) {
    auto j = pos->get_short_jump();
    pos->op = static_cast<Opcode::Op_val>(pos->op + 1);
    pos->get_long_jump() = j + 1;
    update_jumps(ops, pos, 1);
  }

  if (pos->is_short_jump())
    pos->get_short_jump() += val;
  else
    pos->get_long_jump() += val;
}



void update_jumps(std::vector<Opcode>& ops, It pos, int size)
{
  auto offset = 0u;

  for (decltype(ops.rbegin()) back{pos}; back != ops.rend(); ++back) {
    offset += back->size();

    if (back->is_fw_jump() && back->jump_size() > offset)
      jump_set(ops, back.base() - 1, size);
  }

  offset = 0u;

  for (auto fwd = pos + 1; fwd != ops.end(); ++fwd) {

    if (fwd->is_bw_jump() && fwd->jump_size() >= offset)
      jump_set(ops, fwd, size);

    pos += fwd->size();
  }
}

} // namespace



namespace util {

template<typename It>
void write(It&& it, ff7::flevel::script::opcode::Opcode const& op)
{
  auto p = reinterpret_cast<char const*>(&op);
  it = std::copy(p, p + op.size(), it);
}

} // namespace util



ff7::flevel::script::opcode::Opcode::Opcode(util::array_view data) try :
  op{}
{
  auto it = util::make_checked(data);
  op = static_cast<Op_val>(*it++);

  unsigned sz{};

  switch (op) {
    case SPECIAL: {
      auto sp = static_cast<std::uint8_t>(*it);

      if (static_cast<unsigned>(sp - ARROW) > spOpcodeSz.size())
        throw error{"Invalid special opcode"};
      
      sz = spOpcodeSz[sp - ARROW] + 1;
      break;
    }

    case KAWAI:
      sz = static_cast<std::uint8_t>(*it) - 1;

      if (sz > sizeof *this - 1)
        throw error{"Invalid kawaii opcode"};
      break;

    default:
      switch (static_cast<uint8_t>(op)) {
        case 0x0C: case 0x0D:
        case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
        case 0x44: case 0x46: case 0x4C: case 0x4E:
        case 0x61:
        case 0x9D: case 0x9E:
        case 0xBE:
        case 0xD9:
        case 0xF7:
          throw error{"Invalid opcode"};
      }
      sz = opcodeSz[op];
      break;
  }

  std::copy(it, it + sz, reinterpret_cast<char*>(this) + 1);

} catch (std::out_of_range const&) {
  throw error{"Unexpected end of opcode"};
}



ff7::flevel::script::opcode::Opcode::Opcode(): op{} {}



unsigned ff7::flevel::script::opcode::Opcode::size() const
{
  switch (op) {
    case SPECIAL:
      return spOpcodeSz.at(special.op - ARROW) + 2;

    case KAWAI:
      return kawai.len;

    default:
      return opcodeSz.at(op) + 1;
  }
}



char const* ff7::flevel::script::opcode::Opcode::name() const
{
  static constexpr std::array<char const*, 256> const opcode_names
  {
    "ret",      "req",     "reqsw",    "reqew",   "preq",    "prqsw",  "prqew",  "retto",
    "join",     "split",   "sptye",    "gptye",   "unused",  "unused", "dskcg",  "special",
    "jmpf",     "jmpfl",   "jmpb",     "jmpbl",   "ifub",    "ifubl",  "ifsw",   "ifswl",
    "ifuw",     "ifuwl",   "unused",   "unused",  "unused",  "unused", "unused", "unused",
    "minigame", "tutor",   "btmd2",    "btrld",   "wait",    "nfade",  "blink",  "bgmovie",
    "kawai",    "kawaiw",  "pmova",    "slip",    "bgpdh",   "bgscr",  "wcls",   "wsizw",
    "ifkey",    "ifkeyon", "ifkeyoff", "uc",      "pdira",   "ptura",  "wspcl",  "wnumb",
    "sttim",    "goldu",   "goldd",    "chgld",   "hmpmax1", "hmpmax2","mhmmx",  "hmpmax3",
    "message",  "mpara",   "mpara2",   "mpnam",   "unused",  "mpu",    "unused", "mpd",
    "ask",      "menu",    "menu2",    "btltb",   "unused",  "hpu",    "unused", "hpd",
    "window",   "wmove",   "wmode",    "wrest",   "wclse",   "wrow",   "gwcol",  "swcol",
    "stitm",    "dlitm",   "ckitm",    "smtra",   "dmtra",   "cmtra",  "shake",  "nop",
    "mapjump",  "unknown", "scrlc",    "scrla",   "scr2d",   "scrcc",  "scr2dc", "scrlw",
    "scr2dl",   "mpdsp",   "vwoft",    "fade",    "fadew",   "idlck",  "lstmp",  "scrlp",
    "battle",   "btlon",   "btlmd",    "pgtdr",   "getpc",   "pxyzi",  "plusx",  "plus2x",
    "minusx",   "minus2x", "incx",     "inc2x",   "decx",    "dec2x",  "tlkon",  "rdmsd",
    "setbyte",  "setword", "biton",    "bitoff",  "bitxor",  "plus",   "plus2",  "minus",
    "minus2",   "mul",     "mul2",     "div",     "div2",    "mod",    "mod2",   "and",
    "and2",     "or",      "or2",      "xor",     "xor2",    "inc",    "inc2",   "dec",
    "dec2",     "random",  "lbyte",    "hbyte",   "tbyte",   "unknown","unknown","searchx",
    "pc",       "char",    "dfanm",    "anime1",  "visi",    "xyzi",   "xyi",    "xyz",
    "move",     "cmove",   "mova",     "tura",    "animw",   "fmove",  "anime2", "animx1",
    "canim1",   "canmx1",  "msped",    "dir",     "turngen", "turn",   "dira",   "getdir",
    "getaxy",   "getai",   "animx2",   "canim2",  "canmx2",  "asped",  "unused", "cc",
    "jump",     "axyzi",   "lader",    "ofst",    "ofstw",   "talkr",  "slidr",  "solid",
    "prtyp",    "prtym",   "prtye",    "ifprtyq", "ifmembq", "mmbud",  "mmblk",  "mmbuk",
    "line",     "linon",   "mpjpo",    "sline",   "sin",     "cos",    "tlkr2",  "sldr2",
    "pmjmp",    "unknown", "akao2",    "fcfix",   "ccanm",   "animb",  "turnw",  "mppal",
    "bgon",     "bgoff",   "bgrol",    "bgrol2",  "bgclr",   "stpal",  "ldpal",  "cppal",
    "rtpal",    "adpal",   "mppal2",   "stpls",   "ldpls",   "cppal2", "rtpal2", "adpal2",
    "music",    "sound",   "akao",     "musvt",   "musvm",   "mulck",  "bmusc",  "unknown",
    "pmvie",    "movie",   "mvief",    "mvcam",   "fmusc",   "cmusc",  "chmst",  "gameover"
  };



  static constexpr std::array<char const*, 256> const spOpcode_names
  {
                                                             "arrow",  "unused", "unused",
    "smspd",    "flmat",   "flitm",    "btlck",   "mvlck",   "spcnm",  "rsglb",  "clitm"
  };

  return op == SPECIAL ? spOpcode_names.at(special.op - ARROW) : opcode_names.at(op);
}



bool ff7::flevel::script::opcode::Opcode::is_term() const
{
  switch (op) {
    case RET:     case RETTO:    case JMPB: case JMPBL: case MINIGAME:
    case MAPJUMP: case GAMEOVER:
      return true;
    default:
      return false;
  }
}



bool ff7::flevel::script::opcode::Opcode::is_fw_jump() const
{
  switch (op) {
    case JMPF:    case JMPFL:
    case IFUB:    case IFUBL:   case IFSW:     case IFSWL: case IFUW: case IFUWL:
    case IFKEY:   case IFKEYON: case IFKEYOFF:
    case IFPRTYQ: case IFMEMBQ:
      return true;
    default:
      return false;
  }
}



bool ff7::flevel::script::opcode::Opcode::is_bw_jump() const
{
  switch (op) {
    case JMPB: case JMPBL:
      return true;
    default:
      return false;
  }
}



bool ff7::flevel::script::opcode::Opcode::is_short_jump() const
{
  switch (op) {
    case JMPF:    case JMPB:    case IFUB:     case IFSW: case IFUW:
    case IFKEY:   case IFKEYON: case IFKEYOFF:
    case IFPRTYQ: case IFMEMBQ:
      return true;
    default:
      return false;
  }
}



uint8_t& ff7::flevel::script::opcode::Opcode::get_short_jump()
{
  switch (op) {
    case JMPF:
      return jmpf.jump;
    case JMPB:
      return jmpb.jump;
    case IFUB:
      return ifub.jump;
    case IFSW:
      return ifsw.jump;
    case IFUW:
      return ifuw.jump;
    case IFKEY:
      return ifkey.jump;
    case IFKEYON:
      return ifkeyon.jump;
    case IFKEYOFF:
      return ifkeyoff.jump;
    case IFPRTYQ:
      return ifprtyq.jump;
    case IFMEMBQ:
      return ifmembq.jump;
    default:
      throw error{"get_short_jump called on "s + name()};
  }
}



uint16_t& ff7::flevel::script::opcode::Opcode::get_long_jump()
{
  switch (op) {
    case JMPFL:
      return jmpfl.jump;
    case JMPBL:
      return jmpbl.jump;
    case IFUBL:
      return ifubl.jump;
    case IFSWL:
      return ifswl.jump;
    case IFUWL:
      return ifuwl.jump;
    default:
      throw error{"get_long_jump called on "s + name()};
  }
}



unsigned ff7::flevel::script::opcode::Opcode::jump_size() const
{
  switch (op) {
    case JMPF:
      return offsetof(Opcode, jmpf.jump) + jmpf.jump;
    case JMPFL:
      return offsetof(Opcode, jmpfl.jump) +  jmpfl.jump;
    case JMPB:
      return jmpb.jump;
    case JMPBL:
      return jmpbl.jump;
    case IFUB:
      return offsetof(Opcode, ifub.jump) +  ifub.jump;
    case IFUBL:
      return offsetof(Opcode, ifubl.jump) + ifubl.jump;
    case IFSW:
      return offsetof(Opcode, ifsw.jump) + ifsw.jump;
    case IFSWL:
      return offsetof(Opcode, ifswl.jump) + ifswl.jump;
    case IFUW:
      return offsetof(Opcode, ifuw.jump) + ifuw.jump;
    case IFUWL:
      return offsetof(Opcode, ifuwl.jump) + ifuwl.jump;
    case IFKEY:
      return offsetof(Opcode, ifkey.jump) + ifkey.jump;
    case IFKEYON:
      return offsetof(Opcode, ifkeyon.jump) + ifkeyon.jump;
    case IFKEYOFF:
      return offsetof(Opcode, ifkeyoff.jump) + ifkeyoff.jump;
    case IFPRTYQ:
      return offsetof(Opcode, ifprtyq.jump) + ifprtyq.jump;
    case IFMEMBQ:
      return offsetof(Opcode, ifmembq.jump) + ifmembq.jump;
    default:
      throw error{"jump_size called on "s + name()};
  }
}



bool ff7::flevel::script::opcode::Opcode::is_obj_call() const
{
  switch (op) {
    case REQ:   case REQSW: case REQEW:
      return true;
    default:
      return false;
  }
}



bool ff7::flevel::script::opcode::Opcode::is_party_call() const
{
  switch (op) {
    case PREQ:  case PRQSW: case PRQEW:
      return true;
    default:
      return false;
  }
}



bool ff7::flevel::script::opcode::Opcode::is_ret_call() const
{
  return op == RETTO;
}



uint8_t ff7::flevel::script::opcode::Opcode::object() const
{
  switch (op) {
    case REQ:
      return req.obj;
    case REQSW:
      return reqsw.obj;
    case REQEW:
      return reqew.obj;
    case PREQ:
      return preq.obj;
    case PRQSW:
      return prqsw.obj;
    case PRQEW:
      return prqew.obj;
    default:
      throw error{"object called on "s + name()};
  }
}



uint8_t ff7::flevel::script::opcode::Opcode::script() const
{
  switch (op) {
    case REQ:
      return req.script.script();
    case REQSW:
      return reqsw.script.script();
    case REQEW:
      return reqew.script.script();
    case PREQ:
      return preq.script.script();
    case PRQSW:
      return prqsw.script.script();
    case PRQEW:
      return prqew.script.script();
    case RETTO:
      return retto.script.script();
    default:
      throw error{"script called on "s + name()};
  }
}



bool ff7::flevel::script::opcode::Opcode::is_text() const
{
  switch (op) {
    case MESSAGE: case ASK: case MPNAM:
      return true;
    case SPECIAL:
      if (special.op == SPCNM)
        return true;
    default:
      return false;
  }
}



bool ff7::flevel::script::opcode::Opcode::is_message() const
{
  switch (op) {
    case MESSAGE: case ASK:
      return true;
    default:
      return false;
  }
}



uint8_t& ff7::flevel::script::opcode::Opcode::get_text()
{
  switch (op) {
    case MESSAGE:
      return message.text;
    case ASK:
      return ask.text;
    case MPNAM:
      return mpnam.text;
    case SPECIAL:
      if (special.op == SPCNM)
        return special.spcnm.text;
    default:
      throw error{"get_text called on non-text op"};
  } 
}



bool ff7::flevel::script::opcode::Opcode::is_akao() const
{
  switch (op) {
    case MUSIC: case BMUSC:
      return true;
    default:
      return false;
  }
}



uint8_t& ff7::flevel::script::opcode::Opcode::get_akao()
{
  switch (op) {
    case MUSIC:
      return music.track;
    case BMUSC:
      return bmusc.track;
    default:
      throw error{"get_akao called on non-text op"};
  } 
}



bool ff7::flevel::script::opcode::Opcode::is_window() const
{
  switch (op) {
    case WINDOW: case WSIZW:
      return true;
    default:
      return false;
  }
}



uint8_t& ff7::flevel::script::opcode::Opcode::get_window()
{
  switch (op) {
    case WINDOW:
      return window.window;
    case WSIZW:
      return wsizw.window;
    case MESSAGE:
      return message.window;
    case ASK:
      return ask.window;
    default:
      throw error{"window called on non-window op"};
  }
}



uint16_t& ff7::flevel::script::opcode::Opcode::get_x()
{
  switch (op) {
    case WINDOW:
      return window.x;
    case WSIZW:
      return wsizw.x;
    default:
      throw error{"x called on non-window op"};
  }
}



uint16_t& ff7::flevel::script::opcode::Opcode::get_y()
{
  switch (op) {
    case WINDOW:
      return window.y;
    case WSIZW:
      return wsizw.y;
    default:
      throw error{"y called on non-window op"};
  }
}



uint16_t& ff7::flevel::script::opcode::Opcode::get_width()
{
  switch (op) {
    case WINDOW:
      return window.width;
    case WSIZW:
      return wsizw.width;
    default:
      throw error{"width called on non-window op"};
  }
}



uint16_t& ff7::flevel::script::opcode::Opcode::get_height()
{
  switch (op) {
    case WINDOW:
      return window.height;
    case WSIZW:
      return wsizw.height;
    default:
      throw error{"height called on non-window op"};
  }
}



ff7::flevel::script::opcode::Unpacked
ff7::flevel::script::opcode::unpack_ops(Packed const& packed)
{
  Unpacked unpacked;

  for (auto i = 0u; i < packed.size(); ++i)
    for (auto pos = util::make_checked(packed[i]), end = pos + packed[i].size();
      pos != end;)
      try {
        unpacked[i].emplace_back(util::make_view(&*pos, packed[i].size()));
        pos += unpacked[i].back().size();
      } catch (std::exception const& e) {
        throw error{"Opcode error at byte " +
          std::to_string(&*pos - packed[i].data()) + ": " + e.what()
        };
      }

  return unpacked;
}



ff7::flevel::script::opcode::Packed
ff7::flevel::script::opcode::pack_ops(Unpacked const& unpacked)
{
  Packed packed;

  for (auto i = 0u; i < packed.size(); ++i) {
    auto out = back_inserter(packed[i]);
    for (auto& op : unpacked[i])
      util::write(out, op);
  }
  return packed;
}



libff7 ff7::flevel::script::opcode::It ff7::flevel::script::opcode::add_op(
  std::vector<Opcode>& ops, It pos, Opcode const& op)
{
  pos = ops.insert(pos, op);
  update_jumps(ops, pos, op.size());
  return pos;
}



libff7 ff7::flevel::script::opcode::It
ff7::flevel::script::opcode::delete_op(std::vector<Opcode>& ops, It pos)
{
  auto size = pos->size();
  pos = ops.erase(pos);
  update_jumps(ops, pos, -size);
  return pos;
}



void libff7 ff7::flevel::script::opcode::parse(std::vector<Unpacked>& objs,
  Stackwise stack_func, Scriptwise script_func, Opwise op_func)
{

  for (uint8_t obj = 0u; obj < objs.size(); ++obj) {

    Basic_stack stack{nullptr, obj, 0};
    if (stack_func)
      stack_func(stack);
    parse_script(objs, stack, script_func, op_func);

    if (is_model(objs[obj]))
      for (uint8_t scr = 1u; scr < 3u; ++scr) {
        Basic_stack stack{nullptr, obj, scr};
        if (stack_func)
          stack_func(stack);
        parse_script(objs, stack, script_func, op_func);
      }
    
    if (is_line(objs[obj]))
      for (uint8_t scr = 1u; scr < 7u; ++scr) {
        Basic_stack stack{nullptr, obj, scr};
        if (stack_func)
          stack_func(stack);
        parse_script(objs, stack, script_func, op_func);
      }
  }
}

