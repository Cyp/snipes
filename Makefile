
OSRCS = \
colourScheme.c \
graphics.c \
help.c \
keys.c \
main.c \
random.c \
settings.c \
skillLevel.c \
snipes.c \
snipeSound.c

OIMGS = \
bitm8x8.pbm \
bitm10x10.pbm \
bitm12x12.pbm \
bitm14x14.pbm \
bitm16x16.pbm \
bitm20x20.pbm \
bitm24x24.pbm \
bitm25x25.pbm


INTERMEDIATE = intermediate
SRC = src
IMG = img

SRCS = $(OSRCS:%=$(SRC)/%)
OBJ = $(OSRCS:%.c=$(INTERMEDIATE)/%.o)
OBJDEP = $(OSRCS:%.c=$(INTERMEDIATE)/%.d)
IMGS = $(OIMGS:%=$(IMG)/%)
IMGOBJ = $(OIMGS:%.pbm=$(INTERMEDIATE)/%.o)

CFLAGS += `sdl-config --cflags` -I$(INTERMEDIATE)
LDLIBS += `sdl-config --libs`
LDFLAGS += -Wl,-z,noexecstack


all: snipes snipes.6

# Everything at once
#snipes: $(SRCS) $(IMGOBJ)
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS) $(IMGOBJ) $(LDLIBS) -Wall -Wextra

# One file at a time
snipes: $(OBJ) $(IMGOBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(IMGOBJ) $(LDLIBS)

$(INTERMEDIATE)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $< -Wall -Wextra

# Why does this result in an executable stack? Can I run my bitmaps? Do bitmaps behave like Conway's game of life, when run?
$(INTERMEDIATE)/%.o: $(IMG)/%.pbm
	$(LD) -r -b binary -z noexecstack -z really_noexecstack -z pretty_please_noexecstack -o $@ $<

snipes.6: snipes.6.in
	sed s/'`VERSION`'/`cat VERSION`/ < $< > $@

# Release tarball
snipes-%.tar.gz:
	git archive --prefix=$(@:.tar.gz=)/ $(@:snipes-%.tar.gz=%) | gzip -9 > $@
	@stat -c%s $@ ; sha1sum $@
snipes-%.tar.bz2:
	git archive --prefix=$(@:.tar.bz2=)/ $(@:snipes-%.tar.bz2=%) | bzip2 -9 > $@
	@stat -c%s $@ ; sha1sum $@
snipes-%.tar.xz:
	git archive --prefix=$(@:.tar.xz=)/ $(@:snipes-%.tar.xz=%) | xz > $@
	@stat -c%s $@ ; sha1sum $@

clean:
	rm -f snipes $(OBJ) $(IMGOBJ) $(OBJDEP) snipes.6 Makefile.bak

# Dependencies
-include $(OBJDEP)
