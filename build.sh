#!bash/sh

rm -fd test.exe test
g++ --std=gnu++11 -mno-ms-bitfields main.cpp SD.cpp -o test

./test $1
