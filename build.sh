#!bash/sh

rm -f test.exe
g++ --std=gnu++11 -mno-ms-bitfields main.cpp SD.cpp -o test

./test $1
