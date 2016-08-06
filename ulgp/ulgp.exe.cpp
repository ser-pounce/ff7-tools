#include "ff7/lgp.h"
#include "util/filesystem.h"
#include "util/io.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <regex>

namespace fs = boost::filesystem;
using rec_it = fs::recursive_directory_iterator;



int add_file(ff7::lgp::Archive& ar, fs::path const& path)
{
  int added{};

  if (is_directory(path)) {
    for (auto it = rec_it(path); it != rec_it(); ++it)
      if (is_regular_file(it->path())) {
        ar[it->path().generic_string()] = ff7::lgp::make_file(it->path().string());
        ++added;
      }
  
  } else if (is_regular_file(path)) {
    ar[path.generic_string()] = ff7::lgp::make_file(path.string());
    ++added;
  }
  return added;
}



void add(fs::path const& path, std::regex const& pattern)
{
  std::cout << "Adding files..." << std::endl;

  auto ar = exists(path) ?
            ff7::lgp::Archive{path.string()} :
            ff7::lgp::Archive{};

  int added{};

  for (auto it = rec_it("."); it != rec_it(); ++it)
    if (std::regex_match(it->path().filename().string(), pattern))
      added += add_file(ar, it->path().filename());

  std::cout << added << " files added" << std::endl;
  std::cout << "Writing archive to " << path << std::endl;

  auto temp = fs::temp_directory_path() / "temp.lgp";
  ar.write(temp.string());
  ar.clear();

  copy_file(temp, path, fs::copy_option::overwrite_if_exists);
  remove(temp);
}



void extract_file(ff7::lgp::Archive::value_type const& file)
{
  fs::path path{file.first};
  if (path.has_parent_path())
    create_directories(path.parent_path());

  std::ofstream stream(file.first, std::ios::binary);
  util::write(stream, file.second->data());
}



void extract(fs::path const& path, std::regex const& pattern)
{
  std::cout << "Extracting files from " << path << "..." << std::endl;

  ff7::lgp::Archive ar{path.string()};

  int added{};

  for (auto& file : ar) {
    if (std::regex_match(file.first, pattern)) {
      extract_file(file);
      ++added;
    }
  }
  std::cout << added << " files extracted" << std::endl;
}



int main(int argc, char* argv[])
try {

  std::ios_base::sync_with_stdio(false);

  if (argc < 2 || 4 < argc)
    throw std::runtime_error{
      "Usage: ulgp source [destination] [file pattern]"
    };

  fs::path arg1{fs::canonical(argv[1])};
  std::regex pattern{argc == 4 ? argv[3] : ".*"};

  if (fs::is_regular_file(arg1)) {

    auto folder = argc < 3 ? arg1.parent_path() / arg1.stem() : argv[2];
    create_directories(folder);
    current_path(folder);
    extract(arg1, pattern);

  } else if (fs::is_directory(arg1)) {

    auto lgp = argc < 3 ? arg1.string() + ".lgp" : fs::absolute(argv[2]);
    current_path(arg1);
    add(lgp, pattern);

  } else

    throw std::runtime_error{
      "Lgp file / folder not found."
    };

} catch (std::exception const& e) {
  std::cerr << e.what() << '\n';
}

