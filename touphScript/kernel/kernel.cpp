#include "touphScript/kernel/kernel.h"
#include "touphScript/string.h"
#include "ff7/kernel/kernel.h"
#include "ff7/kernel/init_data.h"
#include "util/copy.h"
#include "util/filesystem.h"
#include <fstream>

namespace fs = boost::filesystem;



void touphScript::kernel::dump(std::string const& kernel,
  std::string const& textPath)
{
  std::ifstream file{kernel, file.binary};
  std::vector<char> data{
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };

  auto unc = ff7::kernel::split(data);

  ff7::kernel::Init init;
  util::read(util::make_checked(unc[3]), init);

  std::ofstream out((fs::path{textPath}/"kernel.bin.xml").string(), out.binary);

  out << "<!DOCTYPE kernel SYSTEM \"ts_kernel.dtd\">\n\n"
      << "<kernel>\n\n";

  for (auto i = 0u; i < init.characters.size(); ++i) {
    out << "<name>";
    for (auto c : init.characters[i].name)
      out << string::getc(c);
    out << "</name>\n";
  }

  out << "\n</kernel>\n\n";
}



void touphScript::kernel::encode(std::string const& kernel,
  std::string const& textPath)
{
  namespace rxml = rapidxml;

  std::ifstream file{kernel, file.binary};
  std::vector<char> data{
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };

  std::ifstream xml_file{(fs::path{textPath}/"kernel.bin.xml").string(),
    xml_file.binary};
  std::string xml{
    std::istreambuf_iterator<char>{xml_file}, 
    std::istreambuf_iterator<char>{}
  };

  auto unc = ff7::kernel::split(data);

  ff7::kernel::Init init;
  util::read(util::make_checked(unc[3]), init);

  rxml::xml_document<> doc;
  doc.parse<rxml::parse_default>(&xml[0]);

  auto root = doc.first_node("kernel");
  if (!root)
    throw error{"Root node kernel not found"};

  auto name = root->first_node("name");

  for (auto i = 0u; i < init.characters.size();
  name = name->next_sibling("name"), ++i) {
    if (!name)
      throw error{"Insufficient string tags in kernel"};

    std::vector<char> text;
    string::read_text(name->first_node(), text);

    if (text.size() > init.characters[i].name.size() - 1)
      throw error{"Kernel name " + std::to_string(i) + " too long"};

    text.resize(init.characters[i].name.size(), static_cast<char>(0xFF));
    std::copy(text.begin(), text.end(), init.characters[i].name.data());
  }

  util::write(unc[3].data(), init);
  auto cmp = ff7::kernel::join(unc);

  std::ofstream{kernel, std::ios_base::binary}.write(cmp.data(), cmp.size());
}

