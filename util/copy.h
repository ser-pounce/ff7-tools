#pragma once
#include "util/array_view.h"
#include <cassert>
#include <iterator>
#include <stdexcept>

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

template<typename It, typename T>
std::enable_if_t<is_trivially_copyable_v<T>>
read(It&& it, T& v)
{
  std::copy(it, it + sizeof v, reinterpret_cast<char*>(&v));
  it += sizeof v;
}

template<typename It, typename T, typename A>
std::enable_if_t<is_trivially_copyable_v<T>>
read(It&& it, std::vector<T, A>& vec)
{
  auto end = it + size(vec);
  std::copy(it, end, reinterpret_cast<char*>(vec.data()));
  it = end;
}

template<typename It, typename T, typename A>
std::enable_if_t<!is_trivially_copyable_v<T>>
read(It&& it, std::vector<T, A>& vec)
{
  for (auto& v : vec)
    read(it, v);
}

template<typename It, typename T, size_t sz>
std::enable_if_t<!is_trivially_copyable_v<T>>
read(It&& it, std::array<T, sz>& arr)
{
  for (auto& a : arr)
    read(it, a);
}

template<typename It, typename T, typename... Args>
void read(It&& it, T& val, Args&... args)
{
  read(it, val);
  read(it, args...);
}

template<typename T, typename It>
T get(It&& it)
{
  T v{};
  read(it, v);
  return v; 
}

template<typename It, typename T>
std::enable_if_t<is_trivially_copyable_v<std::remove_reference_t<T>>>
write(It&& it, T const& v)
{
  auto p = reinterpret_cast<char const*>(&v);
  it = std::copy(p, p + sizeof v, it);
}

template<typename It, typename T, typename A>
std::enable_if_t<is_trivially_copyable_v<T>>
write(It&& it, std::vector<T, A> const& vec)
{
  auto p = reinterpret_cast<char const*>(vec.data());
  it = std::copy(p, p + size(vec), it);
}

template<typename It, typename T, typename A>
std::enable_if_t<!is_trivially_copyable_v<T>>
write(It&& it, std::vector<T, A> const& vec)
{
  for (auto& v : vec)
    write(it, v);
}

template<typename It, typename T, size_t sz>
std::enable_if_t<!is_trivially_copyable_v<T>>
write(It&& it, std::array<T, sz> const& arr)
{
  for (auto& a : arr)
    write(a, it);
}

template<typename It, typename T, typename... Args>
void write(It&& it, T const& val, Args const&... args)
{
  write(it, val);
  write(it, args...);
}

template<typename T>
class checked_iterator
{
public:
  using difference_type   = typename std::iterator_traits<T>::difference_type;
  using value_type        = typename std::iterator_traits<T>::value_type;
  using pointer           = typename std::iterator_traits<T>::pointer;
  using reference         = typename std::iterator_traits<T>::reference;
  using iterator_category = typename std::iterator_traits<T>::iterator_category;

  checked_iterator() =default;

  checked_iterator(T pos, T end): begin{pos}, pos{pos}, end{end} {}

  reference operator*() const
  {
    if (pos >= end)
      throw std::out_of_range{"Unexpected end of data"};
    return *pos;
  }
  pointer   operator->() const { return &**this; }
  reference operator[](difference_type d) const { return *(*this + d); }

  checked_iterator& operator+=(difference_type d)
  {
    auto check = pos - begin + d;
    if (check < 0 || end - begin < check)
      throw std::out_of_range{"Invalid pointer offset"};
    pos += d;
    return *this;
  }

  friend difference_type operator-(checked_iterator a, checked_iterator b)
  {
    assert(a.begin == b.begin && a.end == b.end);
    return a.pos - b.pos;
  }

  friend bool operator==(checked_iterator a, checked_iterator b)
  {
    assert(a.begin == b.begin && a.end == b.end);
    return a.pos == b.pos;
  }

private:
  T begin, pos, end;
};

template<typename T>
checked_iterator<T>& operator++(checked_iterator<T>& it) { return it += 1; }

template<typename T>
checked_iterator<T> operator++(checked_iterator<T>& it, int)
{
  auto ret = it;
  ++it;
  return ret;
}

template<typename T>
checked_iterator<T>& operator--(checked_iterator<T>& it) { return it -= 1; }

template<typename T>
checked_iterator<T> operator--(checked_iterator<T>& it, int)
{
  auto ret = it;
  --it;
  return ret;
}

template<typename T> checked_iterator<T> operator+(checked_iterator<T> it,
  typename checked_iterator<T>::difference_type d) { return it += d; }

template<typename T> checked_iterator<T>
operator+(typename checked_iterator<T>::difference_type d, checked_iterator<T> it)
{ return it + d; }

template<typename T> checked_iterator<T> operator-(checked_iterator<T> it,
  typename checked_iterator<T>::difference_type d) { return it + -d; }

template<typename T> checked_iterator<T>& operator-=(checked_iterator<T>& it,
  typename checked_iterator<T>::difference_type d) { return it += -d; }

template<typename T> bool
operator!=(checked_iterator<T> a, checked_iterator<T> b) { return !(a == b); }

template<typename T> bool
operator<(checked_iterator<T> a, checked_iterator<T> b) { return b - a > 0; }

template<typename T> bool
operator>(checked_iterator<T> a, checked_iterator<T> b) { return b < a; }

template<typename T> bool
operator<=(checked_iterator<T> a, checked_iterator<T> b) { return !(a > b); }

template<typename T> bool
operator>=(checked_iterator<T> a, checked_iterator<T> b) { return !(a < b); }

template<typename T>
checked_iterator<T> make_checked(T pos, T end) { return {pos, end}; }

template<typename T> checked_iterator<typename std::vector<T>::const_iterator>
make_checked(std::vector<T> const& vec) { return {vec.begin(), vec.end()}; }

template<typename T> checked_iterator<T const*>
make_checked(util::basic_array_view<T> v) { return {v.begin(), v.end()}; }

} // namespace util

