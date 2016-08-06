libff7_objs := $(addprefix ff7/, \
  gzip.o \
  lgp.o \
  lzss.o \
  lzss.o \
  battle/opcodes.o \
  battle/scene.o \
  flevel/field.o \
  flevel/script/opcodes.o \
  flevel/script/script.o \
  flevel/script/string.o \
  flevel/script/tutorial.o \
  kernel/kernel.o \
  kernel/kernel2.o \
  world/world.o \
)

libff7_path := $(dir $(lastword $(MAKEFILE_LIST)))
libff7_deps := $(libff7_objs:.o=.d)
libff7_dirs := $(addprefix ff7/, battle flevel flevel/script kernel world)

vpath ff7/%.cpp $(libff7_path)..

libff7.so libff7.a: LDFLAGS += $(if $(debug),-g)
libff7.so: CXXFLAGS += -fpic
libff7.so: LDFLAGS  += -shared
libff7.so: $(libff7_objs) libz.so
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

libff7.a: libff7.a($(libff7_objs))

$(libff7_objs): CPPFLAGS += -MMD -MP -I$(libff7_path).. $(if $(debug),,-DNDEBUG)
$(libff7_objs): CXXFLAGS += -std=c++1z -Wall -Wextra -Wpedantic -Weffc++ -Werror \
 	$(if $(debug),-O0 -g,-O3)
$(libff7_objs): | $(libff7_dirs)

$(libff7_dirs): | ff7
$(libff7_dirs) ff7: ; mkdir $@

clean:: mostlyclean
	$(RM) libff7.a libff7.so
mostlyclean:: ; $(RM) $(libff7_objs) $(libff7_deps)

-include $(libff7_deps)

