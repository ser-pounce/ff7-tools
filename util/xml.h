#pragma once
#include <rapidxml/rapidxml.hpp>
#include <limits>
#include <string>
#include <experimental/type_traits>

namespace util {
namespace xml {

using namespace std::string_literals;
using std::experimental::is_arithmetic_v;



struct error : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};



inline auto elem(std::string const& name, std::string const& attributes = {})
{
  return '<' + name + attributes + "/>";
}



inline auto att(std::string const& name, std::string const& value)
{
  return ' ' + name + "=\"" + value + '"';
}



template<typename T>
std::enable_if_t<is_arithmetic_v<T>, std::string>
att(std::string const& name, T value)
{
  return att(name, std::to_string(value));
}



inline rapidxml::xml_attribute<> const* att(rapidxml::xml_node<> const* n, char const* a)
{
  auto attribute = n->first_attribute(a);
  if (!attribute)
    throw error{"No attribute "s + a + " in tag " + n->name()};
  return attribute;
}



inline unsigned long ul(rapidxml::xml_attribute<> const* attribute)
try {
  return std::stoul(attribute->value());
} catch (std::exception const&) {
  throw error{"Invalid value for "s + attribute->name()};
}

template<typename T>
T u(rapidxml::xml_attribute<> const* attribute)
{
  auto i = ul(attribute);
  if (i > std::numeric_limits<T>::max())
    throw error{"Attribute "s + attribute->name() + " out of range"};

  return i;
}

} // namespace xml
} // namespace util

