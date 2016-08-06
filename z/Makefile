libz_objs := $(addprefix z/, \
  adler32.o \
  compress.o \
  crc32.o \
  deflate.o \
  gzclose.o \
  gzlib.o \
  gzread.o \
  gzwrite.o \
  infback.o \
  inffast.o \
  inflate.o \
  inftrees.o \
  trees.o \
  uncompr.o \
  zutil.o \
)

libz_path := $(dir $(lastword $(MAKEFILE_LIST)))
libz_deps := $(libz_objs:.o=.d)

vpath z/%.c $(libz_path)..
vpath z/%.def $(libz_path)..

libz.so libz.a: LDFLAGS += $(if $(debug),-g)
libz.so: CPPFLAGS += $(if $(findstring Windows,$(OS)),,-DHAVE_HIDDEN)
libz.so: CFLAGS   += -fpic
libz.so: LDFLAGS  += -shared
libz.so: $(libz_objs) $(if $(findstring Windows,$(OS)),z/zlib.def)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

libz.a: libz.a($(libz_objs))

$(libz_objs): CPPFLAGS += -MMD -MP -I$(libz_path).. -DZLIB_CONST -DHAVE_UNISTD_H \
	 $(if $(debug),,-DNDEBUG)
$(libz_objs): CFLAGS += -std=c11 $(if $(debug),-O0 -g,-O3)
$(libz_objs): | z

z: ; mkdir $@

clean:: mostlyclean
	$(RM) libz.a libz.so
mostlyclean:: ; $(RM) $(libz_objs) $(libz_deps)

-include $(libz_deps)

