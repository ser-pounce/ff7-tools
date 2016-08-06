#pragma once
#include "ff7/config.h"
#include "util/array_view.h"
#include <functional>
#include <stdexcept>

namespace ff7 {
namespace flevel {
namespace script {
namespace opcode {

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;



struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



#pragma pack(push, 1)
class libff7 Opcode
{
public:
  enum Op_val : uint8_t
  {
    RET, REQ, REQSW, REQEW, PREQ, PRQSW, PRQEW, RETTO,
    JOIN, SPLIT, SPTYE, GPTYE, DSKCG = 0x0E, SPECIAL,
    JMPF, JMPFL, JMPB, JMPBL, IFUB, IFUBL, IFSW, IFSWL,
    IFUW, IFUWL,
    MINIGAME = 0x20, TUTOR, BTMD2, BTRLD, WAIT, NFADE, BLINK, BGMOVIE,
    KAWAI, KAWAIW, PMOVA, SLIP, BGPDH, BGSCR, WCLS, WSIZW,
    IFKEY, IFKEYON, IFKEYOFF, UC, PDIRA, PTURA, WSPCL, WNUMB,
    STTIM, GOLDU, GOLDD, CHGLD, HMPMAX1, HMPMAX2, MHMMX, HMPMAX3,
    MESSAGE, MPARA, MPARA2, MPNAM, MPU = 0x45, MPD = 0x47,
    ASK, MENU, MENU2, BTLTB, HPU = 0x4D, HPD = 0x4F,
    WINDOW, WMOVE, WMODE, WREST, WCLSE, WROW, GWCOL, SWCOL,
    STITM, DLITM, CKITM, SMTRA, DMTRA, CMTRA, SHAKE, NOP,
    MAPJUMP, SCRLO, SCRLC, SCRLA, SCR2D, SCRCC, SCR2DC, SCRLW,
    SCR2DL, MPDSP, VWOFT, FADE, FADEW, IDLCK, LSTMP, SCRLP,
    BATTLE, BTLON, BTLMD, PGTDR, GETPC, PXYZI, PLUSX, PLUS2X,
    MINUSX, MINUS2X, INCX, INC2X, DECX, DEC2X, TLKON, RDMSD,
    SETBYTE, SETWORD, BITON, BITOFF, BITXOR, PLUS, PLUS2, MINUS,
    MINUS2, MUL, MUL2, DIV, DIV2, MOD, MOD2, AND,
    AND2, OR, OR2, XOR, XOR2, INC, INC2, DEC,
    DEC2, RANDOM, LBYTE, HBYTE, TBYTE, SETX, GETX, SEARCHX,
    PC, CHAR, DFANM, ANIME1, VISI, XYZI, XYI, XYZ,
    MOVE, CMOVE, MOVA, TURA, ANIMW, FMOVE, ANIME2, ANIMX1,
    CANIM1, CANMX1, MSPED, DIR, TURNGEN, TURN, DIRA, GETDIR,
    GETAXY, GETAI, ANIMX2, CANIM2, CANMX2, ASPED, CC = 0xBF,
    JUMP, AXYZI, LADER, OFST, OFSTW, TALKR, SLIDR, SOLID,
    PRTYP, PRTYM, PRTYE, IFPRTYQ, IFMEMBQ, MMBUD, MMBLK, MMBUK,
    LINE, LINON, MPJPO, SLINE, SIN, COS, TLKR2, SLDR2,
    PMJMP, PMJMP2, AKAO2, FCFIX, CCANM, ANIMB, TURNW, MPPAL,
    BGON, BGOFF, BGROL, BGROL2, BGCLR, STPAL, LDPAL, CPPAL,
    RTPAL, ADPAL, MPPAL2, STPLS, LDPLS, CPPAL2, RTPAL2, ADPAL2,
    MUSIC, SOUND, AKAO, MUSVT, MUSVM, MULCK, BMUSC, CHMPH,
    PMVIE, MOVIE, MVIEF, MVCAM, FMUSC, CMUSC, CHMST, GAMEOVER
  };

  enum Sp_op_val : uint8_t
  {
    ARROW = 0xF5, PNAME, GMSPD,
    SMSPD, FLMAT, FLITM, BTLCK, MVLCK, SPCNM, RSGLB, CLITM
  };

  struct Bank
  {
    uint8_t first() const { return bank & 0b0000'1111; }
    uint8_t second() const { return (bank & 0b1111'0000) >> 4; }
    uint8_t bank;  
  };

  struct Priority_script
  {
    uint8_t priority() const { return (val & 0b1110'0000) >> 5; }
    uint8_t script() const { return val & 0b0001'1111; }
    uint8_t val;
  };

  struct Req
  {
    uint8_t obj;
    Priority_script script;
  };

  struct Reqsw
  {
    uint8_t obj;
    Priority_script script;
  };

  struct Reqew
  {
    uint8_t obj;
    Priority_script script;
  };

  struct Preq
  {
    uint8_t obj;
    Priority_script script;
  };

  struct Prqsw
  {
    uint8_t obj;
    Priority_script script;
  };

  struct Prqew
  {
    uint8_t obj;
    Priority_script script;
  };

  struct Retto
  {
    Priority_script script;
  };

  struct Join
  {
    uint8_t speed;
  };

  struct Split
  {
    Bank bank_x0y0;
    Bank bank_d0x1;
    Bank bank_y1d1;
    int16_t x0;
    int16_t y0;
    uint8_t direction0;
    int16_t x1;
    int16_t y1;
    uint8_t direction1;
    uint8_t speed;
  };

  struct Sptye
  {
    uint8_t bank_01;
    uint8_t bank_2;
    uint8_t address[3];
  };

  struct Gptye
  {
    uint8_t bank_01;
    uint8_t bank_2;
    uint8_t address[3];
  };

  struct Dskcg
  {
    uint8_t disk;
  };

  struct Jmpf
  {
    uint8_t jump;
  };

  struct Jmpfl
  {
    uint16_t jump;
  };

  struct Jmpb
  {
    uint8_t jump;
  };

  struct Jmpbl
  {
    uint16_t jump;
  };

  struct Ifub
  {
    uint8_t bank_01;
    uint8_t address_0;
    uint8_t value;
    uint8_t op;
    uint8_t jump;
  };

  struct Ifubl
  {
    uint8_t bank_01;
    uint8_t address_0;
    uint8_t value;
    uint8_t op;
    uint16_t jump;
  };

  struct Ifsw
  {
    uint8_t bank_01;
    uint16_t address_0;
    int16_t value;
    uint8_t op;
    uint8_t jump;
  };

  struct Ifswl
  {
    uint8_t bank_01;
    uint16_t address_0;
    int16_t value;
    uint8_t op;
    uint16_t jump;
  };

  struct Ifuw
  {
    uint8_t bank_01;
    uint16_t address_0;
    uint16_t value;
    uint8_t op;
    uint8_t jump;
  };

  struct Ifuwl
  {
    uint8_t bank_01;
    uint16_t address_0;
    uint16_t value;
    uint8_t op;
    uint16_t jump;
  };

  struct Minigame
  {
    uint16_t map;
    int16_t x;
    int16_t y;
    int16_t z;
    uint8_t arg;
    uint8_t game;
  };

  struct Tutor
  {
    uint8_t tutorial;
  };

  struct Btmd2
  {
    uint32_t flags;
  };

  struct Btrld
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Wait
  {
    uint16_t frames;
  };

  struct Nfade
  {
    uint8_t bank_rg;
    uint8_t bank_ba;
    uint8_t bank_s;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
    uint8_t speed;
  };

  struct Blink
  {
    uint8_t off;
  };

  struct Bgmovie
  {
    uint8_t off;
  };

  struct Kawai
  {
    uint8_t len;
    uint8_t op;
    char data[33];
  };

  struct Pmova
  {
    uint8_t member;
  };

  struct Slip
  {
    uint8_t off;
  };

  struct Bgpdh
  {
    uint8_t bank;
    uint8_t layer;
    int16_t depth;
  };

  struct Bgscr
  {
    uint8_t bank_xy;
    uint8_t layer;
    int16_t speed_x;
    int16_t speed_y;
  };

  struct Wcls
  {
    uint8_t window;
  };

  struct Wsizw
  {
    uint8_t window;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
  };

  struct Ifkey
  {
    uint16_t key;
    uint8_t jump;
  };

  struct Ifkeyon
  {
    uint16_t key;
    uint8_t jump;
  };

  struct Ifkeyoff
  {
    uint16_t key;
    uint8_t jump;
  };

  struct Uc
  {
    uint8_t off;
  };

  struct Pdira
  {
    uint8_t character;
  };

  struct Ptura
  {
    uint8_t character;
    uint8_t speed;
    uint8_t direction;
  };

  struct Wspcl
  {
    uint8_t window;
    enum : uint8_t { Clock = 1, Numeric = 2 } type;
    uint8_t x;
    uint8_t y;
  };

  struct Wnumb
  {
    uint8_t bank_01;
    uint8_t window;
    union
    {
      uint32_t value;
      uint16_t address[2];
    };
    uint8_t digits;
  };

  struct Sttim
  {
    uint8_t bank_hm;
    uint8_t bank_s;
    uint8_t h;
    uint8_t m;
    uint8_t s;
  };

  struct Goldu
  {
    uint8_t bank;
    union
    {
      uint8_t address[4];
      uint32_t amount;
    };
  };

  struct Goldd
  {
    uint8_t bank;
    union
    {
      uint8_t address[4];
      uint32_t amount;
    };
  };

  struct Chgld
  {
    uint8_t bank_01;
    uint8_t address[2];
  };

  struct Message
  {
    uint8_t window;
    uint8_t text;
  };

  struct Mpara
  {
    uint8_t bank;
    uint8_t window;
    uint8_t var;
    uint8_t value;
  };

  struct Mpara2
  {
    uint8_t bank;
    uint8_t window;
    uint8_t var;
    uint16_t value;
  };

  struct Mpnam
  {
    uint8_t text;
  };

  struct Mpu
  {
    uint8_t bank;
    uint8_t member;
    uint16_t value;
  };

  struct Mpd
  {
    uint8_t bank;
    uint8_t member;
    uint16_t value;
  };

  struct Ask
  {
    uint8_t bank;
    uint8_t window;
    uint8_t text;
    uint8_t firstline;
    uint8_t lastline;
    uint8_t address;
  };

  struct Menu
  {
    uint8_t bank;
    uint8_t type;
    uint8_t paramter;
  };

  struct Menu2
  {
    uint8_t off;
  };

  struct Btltb
  {
    uint8_t table;
  };

  struct Hpu
  {
    uint8_t bank;
    uint8_t member;
    uint16_t value;
  };

  struct Hpd
  {
    uint8_t bank;
    uint8_t member;
    uint16_t value;
  };

  struct Window
  {
    uint8_t window;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
  };

  struct Wmove
  {
    uint8_t window;
    int16_t x;
    int16_t y;
  };

  struct Wmode
  {
    uint8_t window;
    uint8_t mode;
    uint8_t permanent;
  };

  struct Wrest
  {
    uint8_t window;
  };

  struct Wclse
  {
    uint8_t window;
  };

  struct Wrow
  {
    uint8_t window;
    uint8_t rows;
  };

  struct Gwcol
  {
    uint8_t bank_cr;
    uint8_t bank_gb;
    uint8_t corner;
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  struct Swcol
  {
    uint8_t bank_cr;
    uint8_t bank_gb;
    uint8_t corner;
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  struct Stitm
  {
    uint8_t bank_iq;
    uint16_t item;
    uint8_t quantity;
  };

  struct Dlitm
  {
    uint8_t bank_iq;
    uint16_t item;
    uint8_t quantity;
  };

  struct Ckitm
  {
    uint8_t bank;
    uint16_t item;
    uint8_t address;
  };

  struct Smtra
  {
    uint8_t bank_map0;
    uint8_t bank_ap1ap2;
    uint8_t materia;
    uint8_t ap[3];
  };

  struct Dmtra
  {
    uint8_t bank_map0;
    uint8_t bank_ap1ap2;
    uint8_t materia;
    uint8_t ap[3];
    uint8_t quantity;
  };

  struct Cmtra
  {
    uint8_t bank_map0;
    uint8_t bank_ap1ap2;
    uint8_t bank_a;
    uint8_t materia;
    uint8_t ap[3];
    uint8_t u;
    uint8_t address_a;
  };

  struct Shake
  {
    uint8_t u[2];
    uint8_t swings;
    uint8_t u2[2];
    uint8_t height;
    uint8_t speed;
  };

  struct Mapjump
  {
    uint16_t field;
    int16_t x;
    int16_t y;
    int16_t z;
    uint8_t direction;
  };

  struct Scrlc
  {
    char u[4];
  };

  struct Scrla
  {
    uint8_t bank;
    uint16_t speed;
    uint8_t object;
    uint8_t type;
  };

  struct Scr2d
  {
    uint8_t bank_xy;
    int16_t x;
    int16_t y;
  };

  struct Scr2dc
  {
    uint8_t bank_xy;
    uint8_t bank_s;
    int16_t x;
    int16_t y;
    uint16_t speed;
  };

  struct Scr2dl
  {
    uint8_t bank_xy;
    uint8_t bank_s;
    int16_t x;
    int16_t y;
    uint16_t speed;
  };

  struct Mpdsp
  {
    uint8_t off;
  };

  struct Vwoft
  {
    char u[6];
  };

  struct Fade
  {
    uint8_t bank_rg;
    uint8_t bank_b;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t speed;
    uint8_t type;
    uint8_t mod;
  };

  struct Idlck
  {
    uint16_t triangle;
    uint8_t lock;
  };

  struct Lstmp
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Scrlp
  {
    uint8_t bank;
    uint16_t speed;
    uint8_t member;
    uint8_t type;
  };

  struct Battle
  {
    uint8_t bank;
    uint16_t battle;
  };

  struct Btlon
  {
    uint8_t off;
  };

  struct Btlmd
  {
    char flags[2];
  };

  struct Pgtdr
  {
    uint8_t bank;
    uint8_t member;
    uint8_t address;
  };

  struct Getpc
  {
    uint8_t bank;
    uint8_t member;
    uint8_t address;
  };

  struct Pxyzi
  {
    uint8_t bank_xy;
    uint8_t bank_zi;
    uint8_t member;
    uint8_t address_x;
    uint8_t address_y;
    uint8_t address_z;
    uint8_t address_i;
  };

  struct Plusx
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Plus2x
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct Minusx
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Minus2x
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct Incx
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Inc2x
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Decx
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Dec2x
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Tlkon
  {
    uint8_t off;
  };

  struct Rdmsd
  {
    uint8_t bank;
    uint8_t seed;
  };

  struct Setbyte
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Setword
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct Biton
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t bit;
  };

  struct Bitoff
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t bit;
  };

  struct Bitxor
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t bit;
  };

  struct Plus
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Plus2
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct Minus
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Minus2
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct Mul
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Mul2
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct Div
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Div2
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct Mod
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Mod2
  {
    uint8_t bank_ds;
    uint8_t address_d;
    int16_t value;
  };

  struct And
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct And2
  {
    uint8_t bank_ds;
    uint8_t address;
    uint16_t value;
  };

  struct Or
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Or2
  {
    uint8_t bank_ds;
    uint8_t address;
    uint16_t value;
  };

  struct Xor
  {
    uint8_t bank_ds;
    uint8_t address;
    uint8_t value;
  };

  struct Xor2
  {
    uint8_t bank_ds;
    uint8_t address;
    uint16_t value;
  };

  struct Inc
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Inc2
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Dec
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Dec2
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Random
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Lbyte
  {
    uint8_t bank_ds;
    uint8_t address_d;
    uint8_t address_s;
  };

  struct Hbyte
  {
    uint8_t bank_ds;
    uint8_t address_d;
    uint16_t address_s;
  };

  struct Tbyte
  {
    uint8_t bank_dl;
    uint8_t bank_h;
    uint8_t address;
    uint8_t lo;
    uint8_t hi;
  };

  struct Searchx
  {
    uint8_t bank_sb;
    uint8_t bank_ev;
    uint8_t bank_d;
    uint8_t offset;
    uint16_t begin;
    uint16_t end;
    uint8_t value;
    uint8_t address;
  };

  struct Pc
  {
    uint8_t id;
  };

  struct Char
  {
    uint8_t id;
  };

  struct Dfanm
  {
    uint8_t animation;
    uint8_t speed;
  };

  struct Anime1
  {
    uint8_t animation;
    uint8_t speed;
  };

  struct Visi
  {
    uint8_t on;
  };

  struct Xyzi
  {
    uint8_t bank_xy;
    uint8_t bank_zi;
    int16_t x;
    int16_t y;
    int16_t z;
    uint16_t i;
  };

  struct Xyi
  {
    uint8_t bank_xy;
    uint8_t bank_i;
    int16_t x;
    int16_t y;
    uint16_t i;
  };

  struct Xyz
  {
    uint8_t bank_xy;
    uint8_t bank_z;
    int16_t x;
    int16_t y;
    int16_t z;
  };

  struct Move
  {
    uint8_t bank_xy;
    int16_t x;
    int16_t y;
  };

  struct Cmove
  {
    uint8_t bank_xy;
    int16_t x;
    int16_t y;
  };

  struct Mova
  {
    uint8_t object;
  };

  struct Tura
  {
    uint8_t object;
    uint8_t direction;
    uint8_t steps;
  };

  struct Fmove
  {
    uint8_t bank_xy;
    int16_t x;
    int16_t y;
  };

  struct Anime2
  {
    uint8_t animation;
    uint8_t speed;
  };

  struct Animx1
  {
    uint8_t animation;
    uint8_t speed;
  };

  struct Canim1
  {
    uint8_t animation;
    uint8_t first_frame;
    uint8_t last_frame;
    uint8_t speed;
  };

  struct Canmx1
  {
    uint8_t animation;
    uint8_t first_frame;
    uint8_t last_frame;
    uint8_t speed;
  };

  struct Msped
  {
    uint8_t bank;
    uint16_t speed;
  };

  struct Dir
  {
    uint8_t bank;
    uint8_t direction;
  };

  struct Turngen
  {
    uint8_t bank;
    uint8_t result;
    uint8_t direction;
    uint8_t steps;
    uint8_t type;
  };

  struct Turn
  {
    uint8_t bank;
    uint8_t result;
    uint8_t u;
    uint8_t steps;
    uint8_t type;
  };

  struct Dira
  {
    uint8_t object;
  };

  struct Getdir
  {
    uint8_t bank;
    uint8_t object;
    uint8_t address;
  };

  struct Getaxy
  {
    uint8_t bank_xy;
    uint8_t object;
    uint8_t x;
    uint8_t y;
  };

  struct Getai
  {
    uint8_t bank;
    uint8_t object;
    uint8_t address;
  };

  struct Animx2
  {
    uint8_t animation;
    uint8_t speed;
  };

  struct Canim2
  {
    uint8_t animation;
    uint8_t first_frame;
    uint8_t last_frame;
    uint8_t speed;
  };

  struct Canmx2
  {
    uint8_t animation;
    uint8_t first_frame;
    uint8_t last_frame;
    uint8_t speed;
  };

  struct Asped
  {
    uint8_t bank;
    uint16_t speed;
  };

  struct Cc
  {
    uint8_t object;
  };

  struct Jump
  {
    uint8_t bank_xy;
    uint8_t bank_th;
    int16_t x;
    int16_t y;
    uint16_t triangle;
    uint16_t height;
  };

  struct Axyzi
  {
    uint8_t bank_xy;
    uint8_t bank_zi;
    uint8_t id;
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t triangle;
  };

  struct Lader
  {
    uint8_t bank_xy;
    uint8_t bank_zi;
    int16_t x;
    int16_t y;
    int16_t z;
    uint16_t triangle;
    uint8_t keys;
    uint8_t animation;
    uint8_t direction;
    uint8_t speed;
  };

  struct Ofst
  {
    uint8_t bank_xy;
    uint8_t bank_zs;
    uint8_t type;
    int16_t x;
    int16_t y;
    int16_t z;
    uint16_t speed;
  };

  struct Talkr
  {
    uint8_t bank;
    uint8_t range;
  };

  struct Slidr
  {
    uint8_t bank;
    uint8_t range;
  };

  struct Solid
  {
    uint8_t off;
  };

  struct Prtyp
  {
    uint8_t character;
  };

  struct Prtym
  {
    uint8_t character;
  };

  struct Prtye
  {
    uint8_t character[3];
  };

  struct Ifprtyq
  {
    uint8_t character;
    uint8_t jump;
  };

  struct Ifmembq
  {
    uint8_t character;
    uint8_t jump;
  };

  struct Mmbud
  {
    uint8_t on;
    uint8_t character;
  };

  struct Mmblk
  {
    uint8_t character;
  };

  struct Mmbuk
  {
    uint8_t character;
  };

  struct Line
  {
    int16_t x0;
    int16_t y0;
    int16_t z0;
    int16_t x1;
    int16_t y1;
    int16_t z1;
  };

  struct Linon
  {
    uint8_t on;
  };

  struct Mpjpo
  {
    uint8_t off;
  };

  struct Sline
  {
    uint8_t bank_x0y0;
    uint8_t bank_z0x1;
    uint8_t bank_y1z1;
    int16_t x0;
    int16_t y0;
    int16_t z0;
    int16_t x1;
    int16_t y1;
    int16_t z1;
  };

  struct Sin
  {
    uint8_t bank_01;
    uint8_t bank_2d;
    uint16_t val[3];
    uint8_t address;
  };

  struct Cos
  {
    uint8_t bank_01;
    uint8_t bank_2d;
    uint16_t val[3];
    uint8_t address;
  };

  struct Tlkr2
  {
    uint8_t bank;
    uint16_t range;
  };

  struct Sldr2
  {
    uint8_t bank;
    uint16_t range;
  };

  struct Pmjmp
  {
    uint16_t map;
  };

  struct Akao2
  {
    uint8_t bank_p0p1;
    uint8_t bank_p2p3;
    uint8_t bank_p4;
    uint8_t op;
    uint16_t p[5];
  };

  struct Fcfix
  {
    uint8_t lock;
  };

  struct Ccanm
  {
    uint8_t animation;
    uint8_t speed;
    uint8_t action;
  };

  struct Mppal
  {
    uint8_t bank_sb;
    uint8_t bank_gr;
    uint8_t bank_p;
    uint8_t source;
    uint8_t store;
    uint8_t start_color;
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t palette_size;
  };

  struct Bgon
  {
    uint8_t bank_bl;
    uint8_t background;
    uint8_t layer;
  };

  struct Bgoff
  {
    uint8_t bank_bl;
    uint8_t background;
    uint8_t layer;
  };

  struct Bgrol
  {
    uint8_t bank;
    uint8_t background;
  };

  struct Bgrol2
  {
    uint8_t bank;
    uint8_t background;
  };

  struct Bgclr
  {
    uint8_t bank;
    uint8_t background;
  };

  struct Stpal
  {
    uint8_t bank_pi;
    uint8_t palette;
    uint8_t id;
    uint8_t count;
  };

  struct Ldpal
  {
    uint8_t bank_pi;
    uint8_t palette;
    uint8_t id;
    uint8_t count;
  };

  struct Cppal
  {
    uint8_t bank_sd;
    uint8_t source;
    uint8_t store;
    uint8_t count;
  };

  struct Rtpal
  {
    uint8_t bank_sd;
    uint8_t bank_c;
    uint8_t source;
    uint8_t store;
    uint8_t first_color;
    uint8_t count;
  };

  struct Adpal
  {
    uint8_t bank_sd;
    uint8_t bank_bg;
    uint8_t bank_r;
    uint8_t source;
    uint8_t store;
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t count;
  };

  struct Mppal2
  {
    uint8_t bank_sd;
    uint8_t bank_bg;
    uint8_t bank_r;
    uint8_t source;
    uint8_t store;
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t count;
  };

  struct Stpls
  {
    uint8_t pos;
    uint8_t palette;
    uint8_t first_color;
    uint8_t count;
  };

  struct Ldpls
  {
    uint8_t pos;
    uint8_t palette;
    uint8_t first_color;
    uint8_t count;
  };

  struct Cppal2
  {
    char data[7];
  };

  struct Rtpal2
  {
    char data[7];
  };

  struct Adpal2
  {
    char data[10];
  };

  struct Music
  {
    uint8_t track;
  };

  struct Sound
  {
    uint8_t bank_sp;
    uint16_t sound;
    uint8_t pan;
  };

  struct Akao
  {
    uint8_t bank_p0p1;
    uint8_t bank_p2p3;
    uint8_t bank_p4;
    uint8_t op;
    uint8_t p0;
    uint16_t p[4];
  };

  struct Musvt
  {
    uint8_t track;
  };

  struct Musvm
  {
    uint8_t track;
  };

  struct Mulck
  {
    uint8_t lock;
  };

  struct Bmusc
  {
    uint8_t track;
  };

  struct Pmvie
  {
    uint8_t movie;
  };

  struct Mvief
  {
    uint8_t bank;
    uint8_t address;
  };

  struct Mvcam
  {
    uint8_t off;
  };

  struct Fmusc
  {
    uint8_t track;
  };

  struct Cmusc
  {
    uint8_t bank;
    uint16_t value[2];
  };

  struct Chmst
  {
    uint8_t bank;
    uint8_t address;
  };



  /* Special ops */

  struct Arrow
  {
    uint8_t off;
  };

  struct Smspd
  {
    uint8_t none;
    uint8_t speed;
  };

  struct Btlck
  {
    uint8_t lock;
  };

  struct Mvlck
  {
    uint8_t lock;
  };

  struct Spcnm
  {
    uint8_t character;
    uint8_t text;
  };



  struct Special
  {
    Sp_op_val op;
    union
    {
      Arrow arrow;
      Smspd smspd;
      Btlck btlck;
      Mvlck mvlck;
      Spcnm spcnm;
    };
  };

  Op_val op;
  union
  {
    Req req;
    Reqsw reqsw;
    Reqew reqew;
    Preq preq;
    Prqsw prqsw;
    Prqew prqew;
    Retto retto;
    Join join;
    Split split;
    Sptye sptye;
    Gptye gptye;
    Dskcg dskcg;
    Special special;
    Jmpf jmpf;
    Jmpfl jmpfl;
    Jmpb jmpb;
    Jmpbl jmpbl;
    Ifub ifub;
    Ifubl ifubl;
    Ifsw  ifsw ;
    Ifswl ifswl;
    Ifuw ifuw;
    Ifuwl ifuwl;
    Minigame minigame;
    Tutor tutor;
    Btmd2 btmd2;
    Btrld btrld;
    Wait wait;
    Nfade nfade;
    Blink blink;
    Bgmovie bgmovie;
    Kawai kawai;
    Pmova pmova;
    Slip slip;
    Bgpdh bgpdh;
    Bgscr bgscr;
    Wcls wcls;
    Wsizw wsizw;
    Ifkey ifkey;
    Ifkeyon ifkeyon;
    Ifkeyoff ifkeyoff;
    Uc uc;
    Pdira pdira;
    Ptura ptura;
    Wspcl wspcl;
    Wnumb wnumb;
    Sttim sttim;
    Goldu goldu;
    Goldd goldd;
    Chgld chgld;
    Message message;
    Mpara mpara;
    Mpara2 mpara2;
    Mpnam mpnam;
    Mpu mpu;
    Mpd mpd;
    Ask ask;
    Menu menu;
    Menu2 menu2;
    Btltb btltb;
    Hpu hpu;
    Hpd hpd;
    Window window;
    Wmove wmove;
    Wmode wmode;
    Wrest wrest;
    Wclse wclse;
    Wrow wrow;
    Gwcol gwcol;
    Swcol swcol;
    Stitm stitm;
    Dlitm dlitm;
    Ckitm ckitm;
    Smtra smtra;
    Dmtra dmtra;
    Cmtra cmtra;
    Shake shake;
    Mapjump mapjump;
    Scrlc scrlc;
    Scrla scrla;
    Scr2d scr2d;
    Scr2dc scr2dc;
    Scr2dl scr2dl;
    Mpdsp mpdsp;
    Vwoft vwoft;
    Fade fade;
    Idlck idlck;
    Lstmp lstmp;
    Scrlp scrlp;
    Battle battle;
    Btlon btlon;
    Btlmd btlmd;
    Pgtdr pgtdr;
    Getpc getpc;
    Pxyzi pxyzi;
    Plusx plusx;
    Plus2x plus2x;
    Minusx minusx;
    Minus2x minus2x;
    Incx incx;
    Inc2x inc2x;
    Decx decx;
    Dec2x dec2x;
    Tlkon tlkon;
    Rdmsd rdmsd;
    Setbyte setbyte;
    Setword setword;
    Biton biton;
    Bitoff bitoff;
    Bitxor bitxor;
    Plus plus;
    Plus2 plus2;
    Minus minus;
    Minus2 minus2;
    Mul mul;
    Mul2 mul2;
    Div div;
    Div2 div2;
    Mod mod;
    Mod2 mod2;
    And and_;
    And2 and2;
    Or or_;
    Or2 or2;
    Xor xor_;
    Xor2 xor2;
    Inc inc;
    Inc2 inc2;
    Dec dec;
    Dec2 dec2;
    Random random;
    Lbyte lbyte;
    Hbyte hbyte;
    Tbyte tbyte;
    Searchx searchx;
    Pc pc;
    Char char_;
    Dfanm dfanm;
    Anime1 anime1;
    Visi visi;
    Xyzi xyzi;
    Xyi xyi;
    Xyz xyz;
    Move move;
    Cmove cmove;
    Mova mova;
    Tura tura;
    Fmove fmove;
    Anime2 anime2;
    Animx1 animx1;
    Canim1 canim1;
    Canmx1 canmx1;
    Msped msped;
    Dir dir;
    Turngen turngen;
    Turn turn;
    Dira dira;
    Getdir getdir;
    Getaxy getaxy;
    Getai getai;
    Animx2 animx2;
    Canim2 canim2;
    Canmx2 canmx2;
    Asped asped;
    Cc cc;
    Jump jump;
    Axyzi axyzi;
    Lader lader;
    Ofst  ofst ;
    Talkr talkr;
    Slidr slidr;
    Solid solid;
    Prtyp prtyp;
    Prtym prtym;
    Prtye prtye;
    Ifprtyq ifprtyq;
    Ifmembq ifmembq;
    Mmbud mmbud;
    Mmblk mmblk;
    Mmbuk mmbuk;
    Line line;
    Linon linon;
    Mpjpo mpjpo;
    Sline sline;
    Sin sin;
    Cos cos;
    Tlkr2 tlkr2;
    Sldr2 sldr2;
    Pmjmp pmjmp;
    Akao2 akao2;
    Fcfix fcfix;
    Ccanm ccanm;
    Mppal mppal;
    Bgon bgon;
    Bgoff bgoff;
    Bgrol bgrol;
    Bgrol2 bgrol2;
    Bgclr bgclr;
    Stpal stpal;
    Ldpal ldpal;
    Cppal cppal;
    Rtpal rtpal;
    Adpal adpal;
    Mppal2 mppal2;
    Stpls stpls;
    Ldpls ldpls;
    Cppal2 cppal2;
    Rtpal2 rtpal2;
    Adpal2 adpal2;
    Music music;
    Sound sound;
    Akao akao;
    Musvt musvt;
    Musvm musvm;
    Mulck mulck;
    Bmusc bmusc;
    Pmvie pmvie;
    Mvief mvief;
    Mvcam mvcam;
    Fmusc fmusc;
    Cmusc cmusc;
    Chmst chmst;
  };

  Opcode();
  Opcode(util::array_view data);

  unsigned size() const;
  char const* name() const;
  bool is_term() const;
  bool is_fw_jump() const;
  bool is_bw_jump() const;
  bool is_short_jump() const;
  uint8_t& get_short_jump();
  uint16_t& get_long_jump();
  unsigned jump_size() const;
  bool is_obj_call() const;
  bool is_party_call() const;
  bool is_ret_call() const;
  uint8_t object() const;
  uint8_t script() const;
  bool is_message() const;
  bool is_text() const;
  uint8_t& get_text();
  bool is_akao() const;
  uint8_t& get_akao();
  bool is_window() const;
  uint8_t& get_window();
  uint16_t& get_x();
  uint16_t& get_y();
  uint16_t& get_width();
  uint16_t& get_height();
};
#pragma pack(pop)



using Unpacked = std::array<std::vector<Opcode>, 32>;
using Packed   = std::array<std::vector<char>, 32>;

libff7 Unpacked unpack_ops(Packed const& packed);
libff7 Packed pack_ops(Unpacked const& ops);



using It = std::vector<Opcode>::iterator;

libff7 It add_op(std::vector<Opcode>& ops, It pos, Opcode const& op);
libff7 It delete_op(std::vector<Opcode>& ops, It pos);



class libff7 Stack
{
public:
  virtual uint8_t obj() const =0;
  virtual uint8_t script() const =0;
  virtual bool in(uint8_t obj, uint8_t script) const =0;
  virtual Stack const* parent() const =0;
  virtual ~Stack() =default;
};



using Op_it      = std::vector<Opcode>::iterator;
using Opwise     = std::function<void(Op_it& op, Stack const&)>;
using Scriptwise = std::function<void(Stack const&)>;
using Stackwise  = std::function<void(Stack const&)>;

libff7 void parse(std::vector<Unpacked>& objs, Stackwise stack_func,
  Scriptwise script_func, Opwise op_func);

} // namespace opcode
} // namespace script
} // namespace flevel
} // namespace ff7

