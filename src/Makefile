all: snipes

snipes: snipes.c snipebits.h
	gcc $(CFLAGS) `sdl-config --cflags` -o $@ $< `sdl-config --libs` -Wall

bin2h: bin2h.c
	$(CC) -Os -s -o $@ $< -Wall

bitms.h: bitm8x8.pbm bin2h
	echo "unsigned char bitm8x8[2048]" >> $@
	cat bitm8x8.pbm | tail --bytes=2048 | ./bin2h >> $@
	echo "unsigned char bitm10x10[3200]" >> $@
	cat bitm10x10.pbm | tail --bytes=3200 | ./bin2h >> $@
	echo "unsigned char bitm12x12[4608]" >> $@
	cat bitm12x12.pbm | tail --bytes=4608 | ./bin2h >> $@
	echo "unsigned char bitm14x14[6272]" >> $@
	cat bitm14x14.pbm | tail --bytes=6272 | ./bin2h >> $@
	echo "unsigned char bitm16x16[8192]" >> $@
	cat bitm16x16.pbm | tail --bytes=8192 | ./bin2h >> $@
	echo "unsigned char bitm20x20[12800]" >> $@
	cat bitm20x20.pbm | tail --bytes=12800 | ./bin2h >> $@
	echo "unsigned char bitm24x24[18432]" >> $@
	cat bitm24x24.pbm | tail --bytes=18432 | ./bin2h >> $@
	echo "unsigned char bitm25x25[20000]" >> $@
	cat bitm25x25.pbm | tail --bytes=20000 | ./bin2h >> $@

clean:
	rm -f bin2h bitms.h snipes
