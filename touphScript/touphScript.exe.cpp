#include "touphScript/flevel/flevel.h"
#include "touphScript/world/world.h"
#include "touphScript/kernel/kernel.h"
#include "touphScript/kernel/kernel2.h"
#include "touphScript/battle/scene.h"
#include "util/filesystem.h"
#include <iostream>


int main(int argc, char* argv[])
try {
  namespace fs = boost::filesystem;
  using namespace std::string_literals;
  using namespace touphScript;

  std::ios_base::sync_with_stdio(false);

  if (argc < 4)
    throw std::runtime_error{"Usage: touphscript.exe d|e data_root text_folder"};

  fs::path root{argv[2]};
  root /= "data";

  switch (argv[1][0]) {
    case 'd':
      //flevel::dump((root / "field" / "flevel.lgp").string(), argv[3]);
      //world::dump((root / "wm" / "world_us.lgp").string(), argv[3]);
      //kernel::dump((root / "kernel" / "KERNEL.BIN").string(), argv[3]);
      //kernel2::dump((root / "kernel" / "kernel2.bin").string(), argv[3]);
      battle::dump((root / "battle" / "scene.bin").string(), argv[3]);
      break;
    case 'e':
      //flevel::encode((root / "field" / "flevel.lgp").string(), argv[3]);
      //world::encode((root / "wm" / "world_us.lgp").string(), argv[3]);
      //kernel::encode((root / "kernel" / "KERNEL.BIN").string(), argv[3]);
      //kernel2::encode((root / "kernel" / "kernel2.bin").string(), argv[3]);
      //kernel2::encode((root / "kernel" / "kernel2.bin").string(), argv[3]);
      break;
    default:
      throw std::runtime_error{"Unrecognized option: "s + argv[1][0]};
  }

} catch (std::exception const& e) {
  std::cerr << e.what() << '\n';
}

