
OBJ = snipes.o

BIN2H = ( echo '={' ; od -t d1 -w999999999 | sed 's/^[0-9A-F]* *//;s/  */, /g;s/$$/,/' | head -c-2 ; echo '};' )


all: snipes

snipes: $(OBJ)
	gcc $(LDFLAGS) -o $@ $(OBJ) `sdl-config --libs`

%.o: %.c
	gcc $(CFLAGS) `sdl-config --cflags` -c -o $@ $< -Wall

bin2h: bin2h.c
	$(CC) -Os -s -o $@ $< -Wall

bitms.h: bitm8x8.pbm bitm10x10.pbm bitm12x12.pbm bitm14x14.pbm bitm16x16.pbm bitm20x20.pbm bitm24x24.pbm bitm25x25.pbm
	echo "unsigned char bitm8x8[2048]" >> $@
	@cat bitm8x8.pbm | tail --bytes=2048 | $(BIN2H) >> $@
	echo "unsigned char bitm10x10[3200]" >> $@
	@cat bitm10x10.pbm | tail --bytes=3200 | $(BIN2H) >> $@
	echo "unsigned char bitm12x12[4608]" >> $@
	@cat bitm12x12.pbm | tail --bytes=4608 | $(BIN2H) >> $@
	echo "unsigned char bitm14x14[6272]" >> $@
	@cat bitm14x14.pbm | tail --bytes=6272 | $(BIN2H) >> $@
	echo "unsigned char bitm16x16[8192]" >> $@
	@cat bitm16x16.pbm | tail --bytes=8192 | $(BIN2H) >> $@
	echo "unsigned char bitm20x20[12800]" >> $@
	@cat bitm20x20.pbm | tail --bytes=12800 | $(BIN2H) >> $@
	echo "unsigned char bitm24x24[18432]" >> $@
	@cat bitm24x24.pbm | tail --bytes=18432 | $(BIN2H) >> $@
	echo "unsigned char bitm25x25[20000]" >> $@
	@cat bitm25x25.pbm | tail --bytes=20000 | $(BIN2H) >> $@

clean:
	rm -f bin2h bitms.h snipes $(OBJ)

snipes.o: bitms.h snipebits.h
