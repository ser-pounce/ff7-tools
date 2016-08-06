#include "touphScript/flevel/flevel.h"
#include "touphScript/flevel/field.h"
#include "touphScript/flevel/tutorial.h"
#include "ff7/lgp.h"
#include "ff7/lzss.h"
#include "ff7/flevel/field.h"
#include "ff7/flevel/script/tutorial.h"
#include "util/copy.h"
#include "util/filesystem.h"
#include "util/thread.h"

namespace fs      = boost::filesystem;
namespace lgp     = ff7::lgp;
namespace ff7_tut = ff7::flevel::script::tutorial;


void touphScript::flevel::dump(std::string const& flevel,
  std::string const& textPath)
{
  for (auto& f : lgp::Archive{flevel}) {

    auto xml = (fs::path{textPath} / f.first).string() + ".xml";

    if (ff7::flevel::is_field(f.first))
      field::dump(xml, f.second->data());
    else if (ff7_tut::is_tutorial(f.first))
      tutorial::dump(xml, f.second->data());
  }
}



void touphScript::flevel::encode(std::string const& flevel,
  std::string const& textPath)
{
  auto temp = fs::temp_directory_path() / "temp.lgp";
  {
    auto flevel_write = fs::last_write_time(flevel);
    auto ar = lgp::Archive{flevel};

    util::for_each(ar.begin(), ar.end(), [&](auto& f) {
      fs::path xml{textPath};
      xml /= f.first + ".xml";

      if (is_regular_file(xml) && last_write_time(xml) > flevel_write) {
        if (ff7::flevel::is_field(f.first))
          f.second = lgp::make_file(field::encode(xml.string(), f.second->data()));

        else if (ff7_tut::is_tutorial(f.first))
          f.second = lgp::make_file(tutorial::encode(xml.string()));
      }
    });

    auto temp = fs::temp_directory_path() / "temp.lgp";
    ar.write(temp.string());
  }

  copy_file(temp, flevel, fs::copy_option::overwrite_if_exists);
  remove(temp);
}

