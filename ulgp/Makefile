ulgp_objs  := ulgp/ulgp.exe.o
ulgp_path  := $(dir $(lastword $(MAKEFILE_LIST)))
ulgp_deps  := $(ulgp_objs:.o=.d)
lib_suffix := $(if $(static),a,so)

vpath ulgp/%.cpp $(ulgp_path)..

ulgp.exe: LDFLAGS += $(if $(static),-static) $(if $(debug),-g)
ulgp.exe: $(ulgp_objs) libboost_filesystem.$(lib_suffix) \
	libboost_system.$(lib_suffix) libff7.$(lib_suffix)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(ulgp_objs): CPPFLAGS += -MMD -MP -I$(ulgp_path).. $(if $(static),-DSTATIC_LIB) \
	 $(if $(debug),,-DNDEBUG)
$(ulgp_objs): CXXFLAGS += -std=c++1z -Wall -Wextra -Wpedantic -Weffc++ -Werror \
	 $(if $(debug),-O0 -g,-O3)
$(ulgp_objs): | ulgp

ulgp: ; mkdir $@

clean:: mostlyclean
	$(RM) ulgp.exe
mostlyclean:: ; $(RM) $(ulgp_objs) $(ulgp_deps)

-include $(ulgp_deps)

