libboost_filesystem_objs := $(addprefix boost_filesystem/, \
  codecvt_error_category.o \
  operations.o \
  path.o \
  path_traits.o \
  portability.o \
  unique_path.o \
  utf8_codecvt_facet.o \
  windows_file_codecvt.o \
)

libboost_filesystem_path := $(dir $(lastword $(MAKEFILE_LIST)))
libboost_filesystem_deps := $(libboost_filesystem_objs:.o=.d)

vpath boost_filesystem/%.cpp $(libboost_filesystem_path)..

libboost_filesystem.so libboost_filesystem.a: LDFLAGS += $(if $(debug),-g)
libboost_filesystem.so: CPPFLAGS += -DBOOST_ALL_DYN_LINK
libboost_filesystem.so: CXXFLAGS += -fpic
libboost_filesystem.so: LDFLAGS  += -shared
libboost_filesystem.so: $(libboost_filesystem_objs) libboost_system.so
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

libboost_filesystem.a: libboost_filesystem.a($(libboost_filesystem_objs))

$(libboost_filesystem_objs): CPPFLAGS += -MMD -MP -I$(libboost_filesystem_path).. \
	-DBOOST_NO_AUTO_PTR -DBOOST_SYSTEM_NO_DEPRECATED $(if $(debug),,-DNDEBUG)
$(libboost_filesystem_objs): CXXFLAGS += -std=c++1z $(if $(debug),-O0 -g,-O3)
$(libboost_filesystem_objs): | boost_filesystem

boost_filesystem: ; mkdir $@

clean:: mostlyclean
	$(RM) libboost_filesystem.a libboost_filesystem.so
mostlyclean:: ; $(RM) $(libboost_filesystem_objs) $(libboost_filesystem_deps)

-include $(libboost_filesystem_deps)

