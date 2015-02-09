#/bin/bash

make -C src/utils -f Makefile $1
make -C src -f Makefile $1

