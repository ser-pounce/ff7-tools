// Based on Haruhiko Okumura's LZSS encoder-decoder
#define libff7_source
#include "ff7/lzss.h"
#include "util/copy.h"
#include <climits>

namespace {

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

constexpr int const nib_bit{CHAR_BIT / 2};
constexpr int const minMatch{3};
constexpr int const maxMatch{0xF + minMatch};
constexpr int const bufSize{4096};

constexpr int lo_nib(int c) { return c & 0b0000'1111; }

constexpr int hi_nib(int c) { return c & 0b1111'0000; }

constexpr int lo_byte(int i) { return i & 0b1111'1111; }



class Circular_bintree
{
public:
  Circular_bintree(int nodes): nNodes{nodes} {}

  void insert(char const* val)
  {
    mlen = 0;

    auto search = roots.data() + static_cast<uint8_t>(*val), ins = pos;

    erase();

    if (++pos == nodes.begin() + nNodes)
      pos = nodes.begin();

    ins->val = val;
    int cmp{1};
    
    while (true) {
      if (cmp >= 0) {
        if (search->right)
          search = search->right;
        else {
          search->right = ins;
          ins->parent = search;
          return;
        }
      } else {
        if (search->left)
          search = search->left;
        else {
          search->left = ins;
          ins->parent = search;
          return;
        }
      }

      int i{1};

      for (; i < maxMatch; ++i)
        if ((cmp = ins->val[i] - search->val[i]))
          break;

      if (i > mlen) {
        mlen = i;
        mpos = search->val;
      }

      if (mlen == maxMatch) {
        ins->parent = search->parent;

        if (search->right) {
          search->right->parent = ins;
          ins->right = search->right;
        }

        if (search->left) {
          search->left->parent = ins;
          ins->left = search->left;
        }

        if (search->parent->right == search)
          search->parent->right = ins;
        else
          search->parent->left = ins;

        *search = {};
        return;
      }
    }
  }

  int match_len() const { return mlen; }
  char const* match_pos() const { return mpos; }

private:
  struct Node
  {
    Node* parent, *left, *right;
    char const* val;
  };

  void erase()
  {
    if (!pos->parent)
      return;

    Node* it;

    if (!pos->right)
      it = pos->left;
    else if (!pos->left)
      it = pos->right;

    else {
      it = pos->left;
      
      if (it->right) {
        for (; it->right; it = it->right);
      
        if (it->left)
          it->left->parent = it->parent;

        it->parent->right = it->left;

        it->left = pos->left;
        pos->left->parent = it;
      }
      it->right = pos->right;
      pos->right->parent = it;
    }

    if (it)
      it->parent = pos->parent;

    if (pos->parent->right == pos)
      pos->parent->right = it;
    else
      pos->parent->left = it;

    *pos = {};
  }

  std::array<Node, 256> roots{};
  std::array<Node, bufSize> nodes{};
  int nNodes{};
  Node* pos{nodes.begin()};
  int mlen{};
  char const* mpos{};
};

} // namespace



std::vector<char> ff7::lzss::decompress(util::array_view data)
try {  
  auto it = util::make_checked(data), end = it + data.size();

  if (data.size() - sizeof(uint32_t) != util::get<uint32_t>(it))
    throw error{"Lzss size mismatch"};

  std::vector<char> out(maxMatch);

  for (uint8_t flags{}, flagPos{CHAR_BIT}; it < end; ++flagPos) {
    
    if (flagPos == CHAR_BIT) {
      flags = *it++;
      flagPos = 0;
    }

    if (flags & 1 << flagPos) {

      out.push_back(*it++);

    } else {

      std::size_t offset = static_cast<uint8_t>(*it++);
      auto len           = static_cast<uint8_t>(*it++);

      offset |= hi_nib(len) << nib_bit;
      offset  = out.size() - (out.size() - offset - maxMatch * 2) % bufSize;

      if (out.size() < offset)
        throw error{
          "Invalid lzss offset @ byte " + std::to_string(&*it - data.begin())
        };

      for (len = lo_nib(len) + minMatch; len; --len)
        out.push_back(out[offset++]); 
    }
  }

  out.erase(out.begin(), out.begin() + maxMatch);
  return out;

} catch (std::out_of_range const&) {
  throw error{"Unexpected end of compressed data"};
}



std::vector<char> ff7::lzss::compress(util::array_view data, Compression comp)
{
  std::vector<char> out_vec(sizeof(uint32_t));
  Circular_bintree tree{static_cast<int>(comp)};
  std::vector<char>::size_type flags{};
  uint8_t flagPos{CHAR_BIT};

  auto stop = data.size() < maxMatch ? data.begin() : data.end() - maxMatch;

  for (auto it = data.begin(); it < data.end(); ++it, ++flagPos) {

    if (flagPos == CHAR_BIT) {
      flags = out_vec.size();
      out_vec.push_back(flagPos = 0);
    }

    int len{};

    if (it < stop) {
      tree.insert(it);
      len = tree.match_len();
    }

    if (len >= minMatch) {

      auto pos = tree.match_pos() - data.begin() + bufSize - maxMatch;
      pos %= bufSize;

      out_vec.push_back(lo_byte(pos));
      out_vec.push_back(hi_nib(pos >> nib_bit) | (len - minMatch));
      
      while (--len)
        if (++it < stop)
          tree.insert(it);

    } else {
      out_vec[flags] |= 1 << flagPos;
      out_vec.push_back(*it);
    }
  }

  util::write(out_vec.begin(),
    static_cast<uint32_t>(out_vec.size() - sizeof(uint32_t)));

  return out_vec;
}

