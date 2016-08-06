flevel-tidy_objs  := flevel-tidy/flevel-tidy.exe.o
flevel-tidy_path  := $(dir $(lastword $(MAKEFILE_LIST)))
flevel-tidy_deps  := $(flevel-tidy_objs:.o=.d)
lib_suffix := $(if $(static),a,so)

vpath flevel-tidy/%.cpp $(flevel-tidy_path)..

flevel-tidy.exe: LDFLAGS += $(if $(static),-static) $(if $(debug),-g)
flevel-tidy.exe: $(flevel-tidy_objs) libboost_filesystem.$(lib_suffix) \
	libboost_system.$(lib_suffix) libff7.$(lib_suffix)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(flevel-tidy_objs): CPPFLAGS += -MMD -MP -I$(flevel-tidy_path).. $(if $(static),-DSTATIC_LIB) \
	 $(if $(debug),,-DNDEBUG)
$(flevel-tidy_objs): CXXFLAGS += -std=c++1z -Wall -Wextra -Wpedantic -Weffc++ -Werror \
	 $(if $(debug),-O0 -g,-O3)
$(flevel-tidy_objs): | flevel-tidy

flevel-tidy: ; mkdir $@

clean:: mostlyclean
	$(RM) flevel-tidy.exe
mostlyclean:: ; $(RM) $(flevel-tidy_objs) $(flevel-tidy_deps)

-include $(flevel-tidy_deps)

