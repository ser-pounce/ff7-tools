#define libff7_source
#include "ff7/lgp.h"
#include "util/io.h"
#include "util/string.h"
#include <fstream>
#include <mutex>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::size_t;
using std::experimental::string_view;
using namespace std::string_literals;



namespace {

using Impl = ff7::lgp::Archive::Impl;
using ff7::lgp::error;



#pragma pack(push, 1)

struct Header
{
  Header(std::istream& stream)
  {
    util::read(stream, *this);
    if (magic != squaresoft)
      throw error{"Invalid lgp header"};
  }

  Header(Impl const& ar):
    size{static_cast<uint16_t>(ar.size())} {}

  uint16_t null1{};
  std::array<char, 10> magic = squaresoft;
  uint16_t size{};
  uint16_t null2{};

  static constexpr decltype(magic) const squaresoft{
    'S','Q','U','A','R','E','S','O','F','T'
  };
};

constexpr decltype(Header::squaresoft) const Header::squaresoft;



struct FileInfo
{
  FileInfo() =default;
  FileInfo(string_view const& name):
    name{util::to_array<sizeof this->name>(name)} { }

  std::array<char, 20> name{};
  uint32_t offset{};
  uint8_t type{0xE};
  uint16_t path{};
};



struct HashVal
{
  uint16_t index;
  uint16_t count;
};



struct Path
{
  Path() =default;
  Path(string_view const& path, uint16_t index):
    path{util::to_array<sizeof this->path>(path)}, index{index} {}

  std::array<char, 128> path{};
  uint16_t index{};
};



struct FileHeader
{
  FileHeader() =default;
  FileHeader(FileInfo const& info, std::vector<char> const& data):
    name{info.name}, size{static_cast<uint32_t>(data.size())} {}

  decltype(FileInfo::name) name{};
  uint32_t size{};
};

#pragma pack(pop)



constexpr size_t const hashChars = 30;

using Toc       = std::vector<FileInfo>;
using Paths     = std::vector<std::vector<Path>>;
using HashTable = std::array<HashVal, hashChars * hashChars>;



Toc read_toc(std::istream& stream)
try {
  Header h{stream};
  std::vector<FileInfo> toc(h.size);
  util::read(stream, toc, HashTable{});

  return toc;

} catch (std::ios_base::failure const&) {
  throw error{"Error reading toc"};
}



Paths read_paths(std::istream& stream)
try {
  std::vector<std::vector<Path>> paths;

  for (auto nPaths = util::get<uint16_t>(stream); nPaths; --nPaths) {
    paths.emplace_back(util::get<uint16_t>(stream));
    util::read(stream, paths.back());
  }

  return paths;

} catch (std::ios_base::failure const&) {
  throw error{"Error reading paths"};
}
 


std::string find_path(Paths const& paths, uint16_t pindex, uint32_t tindex)
{
  using util::operator+;

  if (!pindex)
    return {};

  if (pindex > paths.size())
    throw error{
      "Invalid path index for file " + std::to_string(tindex)};

  for (auto& p : paths[pindex - 1])
    if (p.index == tindex)
      return p.path[0] ? p.path + '/' : ""s;

  throw error{"Path not found for file " + std::to_string(tindex)};
}



std::string make_name(Toc const& toc, FileInfo const& t, Paths const& paths)
{
  using util::operator+=;
  auto name = find_path(paths, t.path, std::distance(&toc[0], &t));
  return name += t.name;
}



auto split_path(string_view const& path) noexcept
{
  auto sep = path.rfind('/');
  return sep == path.npos ?
    std::make_pair(string_view{}, path) :
    std::make_pair(path.substr(0, sep), path.substr(sep + 1));
}



auto split_name(string_view const& name) noexcept
{
  auto dot = name.rfind('.');
  return dot == name.npos ?
    std::make_pair(name, string_view{}) :
    std::make_pair(name.substr(0, dot), name.substr(dot + 1));
}



size_t hash(char c)
{
  if (std::isalpha(c))  
    return std::tolower(c) - 'a';
  if (std::isdigit(c))
    return c - '0';
  if (c == '_')
    return 'k' - 'a';
  if (c == '-')
    return 'l' - 'a';
  throw std::out_of_range{""};
}



size_t hash_name(std::string const& name)
try {
  auto path = split_path(name);

  if (path.first.size() > sizeof(Path::path)
  ||  path.second.size() > sizeof(FileInfo::name))
    throw std::out_of_range{""};    
  
  auto stem = split_name(path.second).first;
  auto val  = hash(stem.at(0)) * hashChars;
  if (stem.size() > 1)
    val += hash(stem[1]) + 1;
  return val;

} catch (std::out_of_range const&) {
  throw error{"Invalid filename: " + name};
}



auto make_meta(Impl const& archive)
{
  uint16_t index{};
  Toc toc;
  HashTable table{};
  Paths paths;

  for (Impl::size_type i = 0; i < archive.bucket_count(); ++i) {

    std::unordered_map<string_view, std::vector<Path>> bucket_paths;

    if (archive.bucket_size(i))
      table[i].index = index + 1;

    for (auto it = archive.begin(i); it != archive.end(i); ++it, ++index) {

      ++table[i].count;

      auto path = split_path(it->first);
      toc.emplace_back(path.second);
      bucket_paths[path.second].emplace_back(path.first, index);
    }

    for (auto& path : bucket_paths)
      if (path.second.size() > 1) {
        for (auto const& p : path.second)
          toc[p.index].path = static_cast<uint16_t>(paths.size() + 1);
        paths.push_back(std::move(path.second));
      }
  }
  
  return std::make_tuple(toc, table, paths);
}



void write_paths(std::ostream& out, Paths const& paths)
{
  util::write(out, static_cast<uint16_t>(paths.size()));

  for (auto const& plist : paths)
    util::write(out, static_cast<uint16_t>(plist.size()), plist);
}



void write_files(std::ostream& out, Impl const& archive, Toc& toc)
{
  uint16_t t{};
  for (Impl::size_type i = 0; i < archive.bucket_count(); ++i)
    for (auto it = archive.begin(i); it != archive.end(i); ++it, ++t) {

      auto offset = out.tellp();
      if (out.tellp() > std::numeric_limits<uint32_t>::max())
        throw error{"Archive size limit exceeded"};

      toc[t].offset = static_cast<uint32_t>(offset);

      auto data = it->second->data();
      util::write(out, FileHeader{toc[t], data}, data);
    }
}



class Archive_file final : public ff7::lgp::File
{
  using file_p = std::shared_ptr<std::ifstream>;
public:
  Archive_file(file_p file, uint32_t offset) noexcept
    : file{std::move(file)}, offset{offset} {}

  std::vector<char> data() const override
  {
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard{mutex};

    FileHeader h;
    util::read(file->seekg(offset), h);

    std::vector<char> v(h.size);
    util::read(*file, v);

    return v;

  } catch (std::ios_base::failure const&) {
    file->clear();
    throw error{
      "Error reading file @ offset " + std::to_string(offset)
    };
  }

private:
  mutable file_p file;
  uint32_t offset;
};

} // namespace



ff7::lgp::Archive::Archive(): impl{HashTable{}.size(), hash_name} {}



ff7::lgp::Archive::Archive(std::string const& path) try: Archive{}
{
  auto file = std::make_shared<std::ifstream>(path, std::ios::binary);

  if (!file->is_open())
    throw error{"Could not open lgp file"};

  util::all_exceptions(*file);

  auto const toc   = read_toc(*file);
  auto const paths = read_paths(*file);

  for (auto& t : toc)
    impl.emplace(
      make_name(toc, t, paths),
      std::make_unique<Archive_file>(file, t.offset)
    );
} catch (std::exception const& e) {
  throw error{path + ": " + e.what()};
}



ff7::lgp::File_p const&
ff7::lgp::Archive::operator[](std::string const& name) const
{
  auto f = impl.find(name);
  if (f == impl.end())
    throw error{"File not found: " + name};
  return f->second;
}



void ff7::lgp::Archive::write(std::string const& path)
{
  std::ofstream out(path, out.binary);
  if (!out)
    throw error{"Could not open " + path + " for writing"};

  Header h{impl};
  Toc toc;
  HashTable hashes;
  Paths paths;

  std::tie(toc, hashes, paths) = make_meta(impl);

  util::write(out, h, toc, hashes);
  write_paths(out, paths);
  write_files(out, impl, toc);
  out << "FINAL FANTASY7";
  util::write(out.seekp(sizeof h), toc);
}



ff7::lgp::File_p ff7::lgp::make_file(std::string const& path)
{
  class Concrete_file final : public File
  {
  public:
    Concrete_file(std::string const& path) noexcept: path{path} { }
    std::vector<char> data() const override
    {
      std::ifstream file(path, file.binary);
      if (!file)
        throw std::runtime_error{"Could not open " + path + " for reading"};
      return {util::begin(file), util::end(file)};
    }

  private:
    std::string const path;
  };

  return std::make_unique<Concrete_file>(path);
}



ff7::lgp::File_p ff7::lgp::make_file(std::vector<char> data)
{
  class Mem_file final : public File
  {
  public:
    Mem_file(std::vector<char>&& v): v{v} {}
    std::vector<char> data() const override { return v; }

  private:
    std::vector<char> v;
  };

  return std::make_unique<Mem_file>(move(data));
}

