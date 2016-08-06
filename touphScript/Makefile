touphScript_objs  := $(addprefix touphScript/,\
	touphScript.exe.o \
	string.o \
	battle/scene.o \
	flevel/field.o \
	flevel/flevel.o \
	flevel/tutorial.o \
	kernel/kernel.o \
	kernel/kernel2.o \
	world/world.o \
)

touphScript_path := $(dir $(lastword $(MAKEFILE_LIST)))
touphScript_deps := $(touphScript_objs:.o=.d)
touphScript_dirs := $(addprefix touphScript/, battle flevel kernel world)
lib_suffix := $(if $(static),a,so)

vpath touphScript/%.cpp $(touphScript_path)..

touphScript.exe: LDFLAGS += $(if $(static),-static) $(if $(debug),-g)
touphScript.exe: $(touphScript_objs) libboost_filesystem.$(lib_suffix) \
	libboost_system.$(lib_suffix) libff7.$(lib_suffix) libz.$(lib_suffix)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(touphScript_objs): CPPFLAGS += -MMD -MP -I$(touphScript_path).. $(if $(static),-DSTATIC_LIB) \
	 $(if $(debug),,-DNDEBUG)
$(touphScript_objs): CXXFLAGS += -std=c++1z -Wall -Wextra -Wpedantic -Weffc++ -Werror \
	 $(if $(debug),-O0 -g,-O3)
$(touphScript_objs): | $(touphScript_dirs)

$(touphScript_dirs): | touphScript
$(touphScript_dirs) touphScript: ; mkdir $@

clean:: mostlyclean
	$(RM) touphScript.exe
mostlyclean:: ; $(RM) $(touphScript_objs) $(touphScript_deps)

-include $(touphScript_deps)

