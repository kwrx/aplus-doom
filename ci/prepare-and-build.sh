#!/bin/bash

echo "### Prepare environemnt"

mkdir -p sdk
mkdir -p dist

pushd sdk
wget https://github.com/kwrx/aplus-toolchain/releases/latest/download/x86_64-aplus-toolchain.tar.xz
tar xJf x86_64-aplus-toolchain.tar.xz
popd

export PATH=$(pwd)/sdk/bin:$PATH


echo "### Build"

make CC=x86_64-aplus-gcc -j$(nproc)
make DESTDIR=$(pwd)/dist install


echo "### Package"

tar -C dist -cJf doom.tar.xz
