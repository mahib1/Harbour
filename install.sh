#!/bin/sh


echo "Installer starting"

mkdir -p build/release build/tests build/debug 
cd build/release
cmake ../.. 
make 

echo "==================================================================================="
echo "Install Complete! Please add $(pwd)/bin/release to the PATH"
echo "OR"
echo "Alias $(pwd)/bin/release in the shell rc file"
echo "==================================================================================="
