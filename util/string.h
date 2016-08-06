#pragma once
#include <algorithm>
#include <array>
#include <experimental/string_view>
#include <string>

namespace util {

using std::experimental::string_view;

template<std::size_t sz>
auto end(std::array<char, sz> const& arr) noexcept
{
  return std::find(std::begin(arr), std::end(arr), '\0');
}

template<std::size_t sz>
string_view to_view(std::array<char, sz> const& arr) noexcept
{
  return {arr.data(), static_cast<size_t>(end(arr) - arr.data())};
}

template<std::size_t sz>
std::string to_string(std::array<char, sz> const& arr)
{
  return {arr.data(), static_cast<size_t>(end(arr) - arr.data())};
}

template<std::size_t sz>
auto to_array(string_view const& str)
{
  if (str.size() > sz)
    throw std::out_of_range{""};

  std::array<char, sz> arr{};
  std::copy(str.begin(), str.end(), std::begin(arr));

  return arr;
}

template<std::size_t sz>
auto& operator+=(std::string& str, std::array<char, sz> const& arr) noexcept
{
  return str.append(std::begin(arr), end(arr));  
}

template<std::size_t sz>
auto operator+(std::array<char, sz> const& arr, char c) noexcept
{
  return std::string{std::begin(arr), end(arr)} += c;
}

} // namespace util

