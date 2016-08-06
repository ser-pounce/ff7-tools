#pragma once
#include <array>
#include <experimental/type_traits>
#include <istream>
#include <iterator>
#include <ostream>
#include <vector>

namespace util {

using std::experimental::is_trivially_copyable_v;
using std::size_t;

template<typename T>
std::enable_if_t<is_trivially_copyable_v<T>, size_t>
size(std::vector<T> const& vec) noexcept
{
  return sizeof(T) * vec.size();
}

template<typename T>
std::enable_if_t<!is_trivially_copyable_v<T>, size_t>
size(std::vector<T> const& vec) noexcept
{
  size_t sz{};
  for (auto& v : vec)
    sz += size(v);
  return sz;
}

template<typename T>
std::enable_if_t<is_trivially_copyable_v<T>, std::istream&>
read(std::istream& stream, T& val)
{
  return stream.read(reinterpret_cast<char*>(&val), sizeof val);
}

template<typename T>
std::enable_if_t<is_trivially_copyable_v<T>, std::istream&>
read(std::istream& stream, T&&)
{
  return stream.ignore(sizeof(T));
}

template<typename T>
std::enable_if_t<is_trivially_copyable_v<T>, std::istream&>
read(std::istream& stream, std::vector<T>& vec)
{
  return stream.read(reinterpret_cast<char*>(vec.data()), size(vec));
}

template<typename T>
std::enable_if_t<!is_trivially_copyable_v<T>, std::istream&>
read(std::istream& stream, std::vector<T>& vec)
{
  for (auto& v : vec)
    read(stream, v);
  return stream;
}

template<typename T, size_t sz>
std::enable_if_t<!is_trivially_copyable_v<T>, std::istream&>
read(std::istream& stream, std::array<T, sz>& arr)
{
  for (auto& a : arr)
    read(stream, a);
  return stream;
}

template<typename T, typename... Args>
auto& read(std::istream& stream, T&& val, Args&&... args)
{
  read(stream, std::forward<T>(val));
  return read(stream, std::forward<Args>(args)...);
}

template<typename T>
std::enable_if_t<is_trivially_copyable_v<T>, std::ostream&>
write(std::ostream& stream, T const& val)
{
  return stream.write(reinterpret_cast<char const*>(&val), sizeof val);
}

template<typename T>
std::enable_if_t<is_trivially_copyable_v<T>, std::ostream&>
write(std::ostream& stream, std::vector<T> const& vec)
{
  return stream.write(reinterpret_cast<char const*>(vec.data()), size(vec));
}

template<typename T>
std::enable_if_t<!is_trivially_copyable_v<T>, std::ostream&>
write(std::ostream& stream, std::vector<T> const& vec)
{
  for (auto& v : vec)
    write(stream, v);
  return stream;
}

template<typename T, size_t sz>
std::enable_if_t<!is_trivially_copyable_v<T>, std::ostream&>
write(std::ostream& stream, std::array<T, sz> const& arr)
{
  for (auto& a : arr)
    write(stream, a);
  return stream;
}

template<typename T, typename... Args>
auto& write(std::ostream& buf, T const& val, Args const&... args)
{
  write(buf, val);
  return write(buf, args...);
}

template<typename T>
T get(std::istream& stream)
{
  T val{};
  read(stream, val);
  return val;
}

inline auto begin(std::istream& stream) noexcept
{
  return std::istreambuf_iterator<char>(stream);
}

inline auto end(std::istream&) noexcept
{
  return std::istreambuf_iterator<char>();
}

inline auto all_exceptions(std::ios& ios)
{
  ios.exceptions(ios.badbit | ios.failbit | ios.eofbit);
}

} // namespace util

