#!/bin/bash
echo "----------Make--------"
make; 

echo
echo "----------Creating Files----------"
# cat /dev/random | head -c <bytecount>
cat /dev/random | head -c 10000 > src.dat
echo "src.dat created"
cat /dev/random | head -c 100000 > src_.dat
echo "src_.dat created"

./templateFiles src.dat dst1.dat
./templateFiles src_.dat dst2.dat


make clean;
echo
