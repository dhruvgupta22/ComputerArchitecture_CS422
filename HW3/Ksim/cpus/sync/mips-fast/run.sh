#!/bin/bash

mkdir -p output;

cd ~/Ksim/Bench/testcode/;

cd asm-sim;
gmake clobber clean; gmake;
cd ../c-sim;
gmake clobber clean; gmake;
cd endian;
gmake clobber clean; gmake;
cd ../factorial;
gmake clobber clean; gmake;
cd ../fib;
gmake clobber clean; gmake;
cd ../hello;
gmake clobber clean; gmake;
cd ../host;
gmake clobber clean; gmake;
cd ../ifactorial;
gmake clobber clean; gmake;
cd ../ifib;
gmake clobber clean; gmake;
cd ../log2;
gmake clobber clean; gmake;
cd ../msort;
gmake clobber clean; gmake;
cd ../rfib;
gmake clobber clean; gmake;
cd ../Subreg;
gmake clobber clean; gmake;
cd ../towers;
gmake clobber clean; gmake;
cd ../vadd;
gmake clobber clean; gmake;
cd ..;

cd ~/Ksim/cpus/sync/mips-fast;
gmake clobber clean; gmake;

./mipc ~/Ksim/Bench/testcode/asm-sim/example
mv mipc.log output/asm-sim.log
./mipc ~/Ksim/Bench/testcode/c-sim/example
mv mipc.log output/c-sim.log
./mipc ~/Ksim/Bench/testcode/endian/endian
mv mipc.log output/endian.log
./mipc ~/Ksim/Bench/testcode/factorial/factorial
mv mipc.log output/factorial.log
./mipc ~/Ksim/Bench/testcode/fib/fib
mv mipc.log output/fib.log
./mipc ~/Ksim/Bench/testcode/hello/hello
mv mipc.log output/hello.log
./mipc ~/Ksim/Bench/testcode/host/host
mv mipc.log output/host.log
./mipc ~/Ksim/Bench/testcode/ifactorial/ifactorial
mv mipc.log output/ifactorial.log
./mipc ~/Ksim/Bench/testcode/ifib/ifib
mv mipc.log output/ifib.log
./mipc ~/Ksim/Bench/testcode/log2/log2
mv mipc.log output/log2.log
./mipc ~/Ksim/Bench/testcode/msort/msort
mv mipc.log output/msort.log
./mipc ~/Ksim/Bench/testcode/rfib/rfib
mv mipc.log output/rfib.log
./mipc ~/Ksim/Bench/testcode/Subreg/subreg
mv mipc.log output/subreg.log
./mipc ~/Ksim/Bench/testcode/towers/towers
mv mipc.log output/towers.log
./mipc ~/Ksim/Bench/testcode/vadd/vadd
mv mipc.log output/vadd.log
