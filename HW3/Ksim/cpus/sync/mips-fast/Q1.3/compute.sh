#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: ./analyze_log.sh <input_file.log>"
    exit 1
fi

input_file="$1"
base_name=$(basename "$input_file" .log)
output_file="out_${base_name}.txt"

# Extract values using grep and awk
instructions=$(grep "Number of instructions" "$input_file" | awk '{print $NF}')
int_branches=$(grep "Int Conditional Branches" "$input_file" | awk '{print $NF}')
loads=$(grep "Number of loads" "$input_file" | awk '{print $NF}')
syscall_loads=$(grep "Number of syscall emulated loads" "$input_file" | awk '{print $NF}')
stores=$(grep "Number of stores" "$input_file" | awk '{print $NF}')
syscall_stores=$(grep "Number of syscall emulated stores" "$input_file" | awk '{print $NF}')

# Handle missing values (default to 0)
instructions=${instructions:-0}
int_branches=${int_branches:-0}
loads=${loads:-0}
syscall_loads=${syscall_loads:-0}
stores=${stores:-0}
syscall_stores=${syscall_stores:-0}

# Compute totals
total_instructions=$((instructions + syscall_loads + syscall_stores))
total_loads=$((loads + syscall_loads))
total_stores=$((stores + syscall_stores))

# Avoid division by zero
if [ "$total_instructions" -eq 0 ]; then
    echo "Total instructions is zero — invalid or empty log file."
    exit 1
fi

# Calculate percentages using bc
percent_loads=$(echo "scale=2; 100 * $total_loads / $total_instructions" | bc)
percent_stores=$(echo "scale=2; 100 * $total_stores / $total_instructions" | bc)
percent_branches=$(echo "scale=2; 100 * $int_branches / $total_instructions" | bc)

# Write output
{
    echo "File: $input_file"
    echo "Total Instructions: $total_instructions"
    echo "% Loads: $percent_loads%"
    echo "% Stores: $percent_stores%"
    echo "% Conditional Branches: $percent_branches%"
} > "$output_file"

echo "Results written to $output_file"

