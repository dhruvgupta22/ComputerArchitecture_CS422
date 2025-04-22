#!/bin/bash

folders=(
  asm-sim c-sim endian factorial fib hello host
  ifactorial ifib log2 msort rfib Subreg towers vadd
)

mkdir -p output

for folder in "${folders[@]}"; do
  test_path="$HOME/Ksim/Bench/testcode/$folder"
  (cd "$test_path"; gmake clobber clean; gmake)

  image="$test_path/$folder"

  if [ "$folder" = "asm-sim" ]; then
    image="$test_path/example"
  elif [ "$folder" = "c-sim" ]; then
    image="$test_path/example"
  elif [ "$folder" = "Subreg" ]; then
    image="$test_path/subreg"
  fi

  echo "Checking image: $image"
  if [ ! -f "$image" ]; then
    echo "Image not found for $folder"
    continue
  fi

  cd "$HOME/Ksim/cpus/sync/mips-fast"
  echo "Running mipc with: $image"
  ./mipc "$image"

  if [ ! -f mipc.log ]; then
    echo "mipc.log not generated for $folder"
    continue
  fi

  mv mipc.log "./output/$folder.out"
done
