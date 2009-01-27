
OSRCS = \
colourScheme.c \
graphics.c \
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
IMGH = $(OIMGS:%.pbm=$(INTERMEDIATE)/%.h)

BIN2H = ( echo 'unsigned char $(<:$(IMG)/%.pbm=%)[]={' ; od -t d1 -w999999999 | sed 's/^[0-9A-F]* *//;s/  */, /g;s/$$/,/' | head -c-2 ; echo '};' )

CFLAGS += `sdl-config --cflags` -I$(INTERMEDIATE)
LDLIBS += `sdl-config --libs`


all: snipes snipes.6

# Everything at once
#snipes: $(SRCS) $(INTERMEDIATE)/bitms.h
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS) $(LDLIBS) -Wall -Wextra

# One file at a time
snipes: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

$(INTERMEDIATE)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $< -Wall -Wextra

$(INTERMEDIATE)/%.h: $(IMG)/%.pbm
	tail -n+3 $< | $(BIN2H) >> $@

$(INTERMEDIATE)/graphics.o: $(INTERMEDIATE)/bitms.h
$(INTERMEDIATE)/bitms.h: $(IMGH)
	cat $(IMGH) > $@

snipes.6: snipes.6.in
	sed s/'`VERSION`'/`cat VERSION`/ < $< > $@

# Release tarball
snipes-%.tar.gz:
	git archive --prefix=$(@:.tar.gz=)/ $(@:snipes-%.tar.gz=%) | gzip -9 > $@
	@stat -c%s $@ ; sha1sum $@
snipes-%.tar.bz2:
	git archive --prefix=$(@:.tar.bz2=)/ $(@:snipes-%.tar.bz2=%) | bzip2 -9 > $@
	@stat -c%s $@ ; sha1sum $@
snipes-%.tar.lzma:
	git archive --prefix=$(@:.tar.lzma=)/ $(@:snipes-%.tar.lzma=%) | lzma > $@
	@stat -c%s $@ ; sha1sum $@

clean:
	rm -f $(INTERMEDIATE)/bitms.h snipes $(OBJ) $(OBJDEP) $(IMGH) Makefile.bak

# Dependencies
-include $(OBJDEP)
