#include "ff7/lgp.h"
#include "ff7/lzss.h"
#include "ff7/flevel/field.h"
#include "ff7/flevel/script/opcodes.h"
#include "ff7/flevel/script/script.h"
#include "ff7/flevel/script/string.h"
#include "ff7/flevel/script/akao.h"
#include "util/copy.h"
#include "util/filesystem.h"
#include "util/thread.h"
#include <iostream>
#include <future>


namespace {

namespace scr = ff7::flevel::script;
namespace op  = scr::opcode;




std::vector<op::Unpacked> unpack_scripts(scr::Script const& script)
{
  std::vector<op::Unpacked> objs(script.objects.size());

  for (auto o = 0u; o < script.objects.size(); ++o)
    objs[o] = op::unpack_ops(script.objects[o].second);

  return objs;
}



void pack_scripts(scr::Script& script, std::vector<op::Unpacked> const& objs)
{
  for (auto o = 0u; o < script.objects.size(); ++o)
    script.objects[o].second = op::pack_ops(objs[o]);
}



void strip_scripts(std::vector<op::Unpacked>& objs)
{
  std::vector<op::Unpacked> stripped(objs.size());

  auto script_func = [&](op::Stack const& stack) {
    auto obj = stack.obj(), script = stack.script();
    if (stripped[obj][script].empty())
      stripped[obj][script] = objs[obj][script];
  };
  
  parse(objs, nullptr, script_func, nullptr);

  objs = std::move(stripped);
}



void strip_text(std::vector<op::Unpacked>& objs,
  std::vector<std::vector<char>>& text)
{
  uint8_t pos{};
  std::unordered_map<uint8_t, uint8_t> map;

   auto op_func = [&](op::Op_it& op, op::Stack const& stack) {

    if (!op->is_text())
      return;

    auto& id = op->get_text();

    if (!map.count(id)) {
      if (id < text.size())
        map[id] = pos++;
      else
        op = --delete_op(objs[stack.obj()][stack.script()], op);
    }
  };

  parse(objs, nullptr, nullptr, op_func);

  for (auto& obj : objs)
    for (auto& script : obj)
      for (auto& op : script)
        if (op.is_text())
          op.get_text() = map[op.get_text()];

  std::vector<std::vector<char>> sorted(pos);
  for (auto& p : map)
    sorted[p.second] = move(text[p.first]);
  text = std::move(sorted);
}



void strip_extra(std::vector<op::Unpacked>& objs,
  std::vector<std::vector<char>>& extra)
{
  std::vector<std::vector<char>> sorted;
  std::unordered_map<uint8_t, uint8_t> map;

  for (auto& obj : objs)
    for (auto& script : obj)
      for (auto op = script.begin(); op != script.end(); ++op)

        if (op->is_akao()) {
          auto& id = op->get_akao();
          if (!map.count(id)) {
            if (id < extra.size() && scr::akao::valid(extra[id])) {
              map[id] = sorted.size();
              sorted.push_back(move(extra[id]));
            } else {
              op = --delete_op(script, op);
              continue;
            }
          }
          id = map[id];
        }
  extra = std::move(sorted);
}



void add_windows(std::vector<op::Unpacked>& objs)
{
  std::array<std::pair<bool, op::Opcode>, 4> windows;

  auto stack_func = [&](op::Stack const&) {
    for (auto i = 0u; i < windows.size(); ++i) {
      windows[i].first = false;
      windows[i].second.op = op::Opcode::WINDOW;
      windows[i].second.window.window = i;
      windows[i].second.window.x = 80;
      windows[i].second.window.y = 80;
      windows[i].second.window.width = 157;
      windows[i].second.window.height = 32;
    }
  };
  
  auto op_func = [&](op::Op_it& op, op::Stack const& stack) {
    if (op->is_window())
      windows.at(op->get_window()) = std::make_pair(true, *op);
    else if (op->is_message()) {
      if (!windows.at(op->get_window()).first)
        op = --add_op(objs[stack.obj()][stack.script()], op,
          windows[op->get_window()].second);
      else
        windows[op->get_window()].first = false;
    }
  };

  parse(objs, stack_func, nullptr, op_func);
}

} // namespace



int main(int argc, char* argv[])
try {
  using namespace ff7;
  namespace fs = boost::filesystem;
  namespace fl = flevel;

  if (argc != 2)
    throw std::runtime_error{"Usage: flevel-tidy.exe flevel"};

  auto temp = fs::temp_directory_path() / "temp.lgp";
  {
    auto flevel = lgp::Archive(argv[1]);

    util::for_each(flevel.begin(), flevel.end(), [](auto& f) {
      if (fl::is_field(f.first))
        try {
          auto field   = fl::Field{lzss::decompress(f.second->data())};
          auto script  = fl::script::Script{field[field.Section::script]};
          auto objects = unpack_scripts(script);

          strip_scripts(objects);
          strip_text(objects, script.text);
          strip_extra(objects, script.extra);
          add_windows(objects);

          pack_scripts(script, objects);

          field[field.Section::script] = script.write();

          f.second = lgp::make_file(lzss::compress(field.write()));

        } catch (std::exception const& e) {
          throw std::runtime_error{f.first + ": " + e.what()};
        }
      });

    flevel.write(temp.string());
  }

  copy_file(temp, argv[1], fs::copy_option::overwrite_if_exists);
  remove(temp);

} catch (std::exception const& e) {
  std::cerr << e.what();
}

