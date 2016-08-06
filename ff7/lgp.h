#pragma once
#include "ff7/config.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ff7 {
namespace lgp {

struct libff7 error final : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



class libff7 File
{
public:
  virtual ~File() =default;
  virtual std::vector<char> data() const =0;
};

using File_p  = std::unique_ptr<File>;

libff7 File_p make_file(std::string const& path);
libff7 File_p make_file(std::vector<char> data);



class libff7 Archive
{
public:
  using Hasher         = std::size_t(&)(std::string const&);
  using Impl           = std::unordered_map<std::string, File_p, Hasher>;
  using value_type     = Impl::value_type;
  using iterator       = Impl::iterator;
  using const_iterator = Impl::const_iterator;

  Archive();
  Archive(std::string const& path);

  std::size_t size() const { return impl.size(); }

  File_p const& operator[](std::string const& name) const;
  File_p& operator[](std::string const& name) { return impl[name]; }
  
  const_iterator begin() const { return impl.begin(); }
  const_iterator end() const { return impl.end(); }
  const_iterator cbegin() const { return impl.cbegin(); }
  const_iterator cend() const { return impl.cend(); }
  iterator begin() { return impl.begin(); }
  iterator end() { return impl.end(); }
  
  void write(std::string const& path);

  void clear() { impl.clear(); }

private:
  Impl impl;
};

} // namespace lgp
} // namespace ff7

