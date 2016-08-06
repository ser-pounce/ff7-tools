#define libff7_source
#include "ff7/flevel/script/script.h"
#include "ff7/flevel/script/opcodes.h"
#include "ff7/flevel/script/string.h"
#include "util/copy.h"
#include "util/string.h"

using std::uint16_t;
using std::uint32_t;

namespace {

using Offsets = std::array<uint16_t, 32>;
using namespace ff7::flevel::script;



#pragma pack(push, 1)
struct Header
{
  uint16_t magic{0x0502};
  uint8_t nObjects{};
  uint8_t nModels{};
  uint16_t textOffset{};
  uint16_t nExtraBlocks{};
  uint16_t scale{};
  uint16_t null1[3]{};
  Script::String creator{};
  Script::String name{};
};
#pragma pack(pop)



std::vector<char> read_script(util::array_view data, bool init)
{
  auto pos = make_checked(data), end{pos};
  
  while (true)
    try {
      opcode::Opcode op{{&*pos, data.size()}};

      if (op.is_fw_jump()) {
        auto jump = op.jump_size();
        if (pos + jump > end)
          end = pos + jump;
      }

      if (op.is_term() && pos >= end) {
        if (init)
          init = false;
        else {
          pos += op.size();
          break;
        }
      }

      pos += op.size();

    } catch (std::exception const& e) {
      throw error{"Opcode error at byte " +
        std::to_string(&*pos - data.begin()) + ": " + e.what()
      };
    }

  return {data.begin(), &*pos};
}



Script::Scripts read_scripts(util::array_view data, Offsets const& offsets)
{
  Script::Scripts scripts;

  for (auto i = 0u; i < offsets.size(); ++i)
    if (!i || offsets[i] != offsets[i - 1]) // Empty scripts point to previous
      try {
        scripts[i] = read_script({data.begin() + offsets[i], data.size()}, !i);
      } catch (std::exception const& e) {
        throw error{"Script " + std::to_string(i) + ": " + e.what()};
      }

  return scripts;
}



std::vector<Script::Object> read_objects(util::array_view data,
  std::vector<Script::String> const& names, std::vector<Offsets> const& offsets)
{
  std::vector<Script::Object> objects(offsets.size());

  for (auto i = 0u; i < offsets.size(); ++i)
    try {
      objects[i] = {names[i], read_scripts(data, offsets[i])};
    } catch (std::exception const& e) {
      throw error{util::to_string(names[i]) + ": " + e.what()};
    }

  return objects;
}



std::vector<std::vector<char>> read_extra_blocks(util::array_view data,
  std::vector<uint32_t> const& offsets)
{
  std::vector<std::vector<char>> out;

  for (auto i = 0u; i < offsets.size() - 1; ++i) {
    std::vector<char> block(offsets[i + 1] - offsets[i]);
    util::read(util::make_checked(data) + offsets[i], block);
    out.push_back(block);
  }
    
  return out;
}

} // namespace



ff7::flevel::script::Script::Script(util::array_view data) try :
nModels{}, scale{}, creator{}, name{}, objects{}, text{}, extra{}
{
  auto it = util::make_checked(data);

  Header h;
  util::read(it, h);
  if (h.magic != Header{}.magic)
    throw error{"Invalid script header"};

  nModels = h.nModels;
  scale   = h.scale;
  creator = h.creator;
  name    = h.name;

  std::vector<String> object_names(h.nObjects);
  std::vector<uint32_t> extra_offsets(h.nExtraBlocks);
  std::vector<Offsets> script_offsets(h.nObjects);

  util::read(it, object_names, extra_offsets, script_offsets);

  objects = read_objects(data, object_names, script_offsets);

  auto textEnd = extra_offsets.empty() ? data.size() : extra_offsets.front();
  
  if (h.textOffset + 5u <= textEnd) // Min text table is 5 bytes
    text = string::read_table({data.begin() + h.textOffset, textEnd});
  
  extra_offsets.push_back(data.size());
  extra = read_extra_blocks(data, extra_offsets);

} catch (std::out_of_range const&) {
  throw error{"Unexpected end of script section"};
} catch (std::exception const& e) {
  throw error{e.what()};
}



std::vector<char> ff7::flevel::script::Script::write() const
{
  Header h;
  h.nObjects     = objects.size();
  h.nModels      = nModels;
  h.nExtraBlocks = extra.size();
  h.scale        = scale;
  h.creator      = creator;
  h.name         = name;

  std::vector<String> names(h.nObjects);
  std::vector<uint32_t> extra_offsets(h.nExtraBlocks);
  std::vector<Offsets> script_offsets(h.nObjects);

  std::vector<char> out;
  auto it = std::back_inserter(out);

  util::write(it, h, names, extra_offsets, script_offsets);

  for (auto obj = 0u; obj < objects.size(); ++obj) {
    
    names[obj] = objects[obj].first;

    if (objects[obj].second[0].empty())
      throw error{"Empty init script in " + util::to_string(objects[obj].first)};

    for (auto script = 0u; script < script_offsets[obj].size(); ++script)
      if (!objects[obj].second[script].empty()) {
        script_offsets[obj][script] = out.size();
        util::write(it, objects[obj].second[script]);
      }
      else
        script_offsets[obj][script] = script_offsets[obj][script - 1];
  }

  h.textOffset = out.size();

  std::vector<std::uint16_t> text_offsets{static_cast<uint16_t>(text.size())};
  std::uint16_t off = sizeof(uint16_t) * (text.size() + 1);

  for (auto& t : text) {
    text_offsets.push_back(off);
    off += t.size();
  }

  util::write(it, text_offsets, text);

  for (auto a = 0u; a < extra.size(); ++a) {
    extra_offsets[a] = out.size();
    util::write(it, extra[a]);
  }

  util::write(out.begin(), h, names, extra_offsets, script_offsets);

  return out;
}

