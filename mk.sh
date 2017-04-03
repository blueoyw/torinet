#!/bin/bash

cd lib
make clean
make -j3
make install

cd ../client
make clean
make -j3
make install

cd ../server
make clean
make -j3
make install

test
