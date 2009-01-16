gcc bin2h.c -o bin2h
./bitms.sh
gcc -O3 -fomit-frame-pointer -s -I/usr/include/SDL snipes.c -lSDL -o snipes
echo .
echo .
echo ...
sleep 1
echo ...
echo .
echo .
./snipes 
