#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#define BOOST_NO_AUTO_PTR
#define BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#ifndef STATIC_LIB
#define BOOST_ALL_DYN_LINK
#endif
#include <boost/filesystem.hpp>
#undef BOOST_NO_AUTO_PTR
#undef BOOST_SYSTEM_NO_DEPRECATED
#undef BOOST_FILESYSTEM_NO_DEPRECATED
#ifndef STATIC_LIB
#undef BOOST_ALL_DYN_LINK
#endif
#pragma GCC diagnostic pop

