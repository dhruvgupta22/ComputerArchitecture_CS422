#!/bin/bash

# List of folder names
folders=(
  asm-sim c-sim endian factorial fib hello host
  ifactorial ifib log2 msort rfib Subreg towers vadd
)

# Output directory
mkdir -p output

# Run simulation for each folder
for folder in "${folders[@]}"; do
  # Get the image file name (based on folder name)
  image="$HOME/Ksim/Bench/testcode/$folder/$folder"

  # Special case: 'fact' folder has file named 'factorial.image'
  if [ "$folder" = "asm-sim" ]; then
    image="$HOME/Ksim/Bench/testcode/asm-sim/example"
  fi
  if [ "$folder" = "c-sim" ]; then
    image="$HOME/Ksim/Bench/testcode/c-sim/example"
  fi
  if [ "$folder" = "Subreg" ]; then
    image="$HOME/Ksim/Bench/testcode/Subreg/subreg"
  fi

  # Run simulation
  ./mipc "$image"

  # Move output file
  mv mipc.log "./output/$folder.out"
done