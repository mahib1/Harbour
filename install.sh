#!/bin/sh


echo "Installer starting"

mkdir -p build/release build/tests build/debug 
cd build/release
cmake ../.. 
make 

echo "Install Complete! Please add $(pwd)/bin/release to the PATH"
