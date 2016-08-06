libboost_system_objs := boost_system/error_code.o
libboost_system_path := $(dir $(lastword $(MAKEFILE_LIST)))
libboost_system_deps := $(libboost_system_objs:.o=.d)

vpath boost_system/%.cpp $(libboost_system_path)..

libboost_system.so libboost_system.a: LDFLAGS += $(if $(debug),-g)
libboost_system.so: CPPFLAGS += -DBOOST_ALL_DYN_LINK
libboost_system.so: CXXFLAGS += -fpic
libboost_system.so: LDFLAGS  += -shared
libboost_system.so: $(libboost_system_objs)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

libboost_system.a: libboost_system.a($(libboost_system_objs))

$(libboost_system_objs): CPPFLAGS += -MMD -MP -I$(libboost_system_path).. \
	$(if $(debug),,-DNDEBUG)
$(libboost_system_objs): CXXFLAGS += -std=c++1z $(if $(debug),-O0 -g,-O3)
$(libboost_system_objs): | boost_system

boost_system: ; mkdir $@

clean:: mostlyclean
	$(RM) libboost_system.a libboost_system.so
mostlyclean:: ; $(RM) $(libboost_system_objs) $(libboost_system_deps)

-include $(libboost_system_deps)

