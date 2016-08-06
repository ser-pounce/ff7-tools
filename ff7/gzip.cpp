#define libff7_source
#define ZLIB_CONST
#include "ff7/gzip.h"
#include <zlib.h>

std::vector<char>
ff7::gzip::decompress(util::array_view data, std::uint32_t read)
{
  std::vector<char> unc(read);

	z_stream gzStrm{};
	gzStrm.next_in  = reinterpret_cast<uint8_t const*>(data.begin());
	gzStrm.avail_in = data.size();
	
	gzStrm.next_out  = reinterpret_cast<uint8_t*>(unc.data());
	gzStrm.avail_out = unc.size();
	
	if (inflateInit2(&gzStrm, MAX_WBITS + 16))
		throw error{"gzip init error"};

	auto ret = inflate(&gzStrm, Z_SYNC_FLUSH);
  if (ret != Z_OK && ret != Z_STREAM_END)
		throw error{"gzip decompression error"};

	if (inflateEnd(&gzStrm))
		throw error{"gzip de-init error"};

	return unc;
}



std::vector<char> ff7::gzip::compress(util::array_view data)
{
	std::vector<char> cmp(data.size() + 5 * (data.size() / 16383) + 1);
	
	z_stream gzStrm{};
	gzStrm.next_in  = reinterpret_cast<uint8_t const*>(data.begin());
	gzStrm.avail_in = data.size();
	
	gzStrm.next_out  = reinterpret_cast<uint8_t*>(cmp.data());
	gzStrm.avail_out = cmp.size();
	
	
	if (deflateInit2(&gzStrm, Z_BEST_COMPRESSION, Z_DEFLATED,
			MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY)
		|| deflate(&gzStrm, Z_FINISH) != Z_STREAM_END
		|| deflateEnd(&gzStrm))
		throw error("gzip compression error");

	cmp.resize(cmp.size() - gzStrm.avail_out);
	return cmp;
}

