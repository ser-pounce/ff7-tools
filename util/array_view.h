#pragma once
#include <array>
#include <experimental/type_traits>
#include <vector>

namespace util {

using std::experimental::is_same_v;

template<typename It>
using it_noref = std::iterator_traits<std::remove_reference_t<It>>;

template<typename It>
using it_traits_vt = typename it_noref<It>::value_type;

template<typename It>
using it_traits_cat = typename it_noref<It>::iterator_category;

template<typename T>
class basic_array_view
{
public:
  basic_array_view(T const* pos, std::uint64_t len): pos{pos}, len{len} {}
  basic_array_view(T const* pos, T const* end):
    pos{pos}, len{static_cast<std::uint64_t>(end - pos)} {}

  template<typename It, typename = std::enable_if_t<
    is_same_v<it_traits_cat<It>, std::random_access_iterator_tag>>>
  basic_array_view(It pos, std::uint64_t len):
    basic_array_view{&*pos, len} {}

  template<typename A> basic_array_view(std::vector<T, A> const& v):
    basic_array_view{v.data(), v.size()} {}

  template<std::size_t sz> basic_array_view(std::array<T, sz> const& a):
    basic_array_view{a.data(), a.size()} {}

  T const* begin() const { return pos; }
  T const* end() const { return pos + len; }
  std::uint64_t size() const { return len; }

private:
  T const* pos;
  std::uint64_t len;
};

using array_view = basic_array_view<char>;

template<typename It> basic_array_view<it_traits_vt<It>>
make_view(It pos, std::uint64_t len) { return {pos, len}; }

template<typename T, typename A>
basic_array_view<T> make_view(std::vector<T, A> const& v) { return {v}; }

template<typename T, std::size_t sz>
basic_array_view<T> make_view(std::array<T, sz> const& a) { return {a}; }

} // namespace util

