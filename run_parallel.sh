#!/bin/bash

# Set the directory
dir_path="cnf_benchmarks"
time_path="perf/parallel.txt"

# Set the program
prog="./parallel"

microtime() {
    python -c 'import time; print(time.time())'
}

# Clear time records
> $time_path

# Loop through all files in the directory
for file in "$dir_path"/*
do
    # Check if it is a file (not a directory)
    if [ -f "$file" ]
    then
		for i in {1..10}
		do
			# Total running time of this file
			total_time=0
			# Run the program with the file as an argument
			START=$(microtime)
			$prog "$file" > /dev/null
			END=$(microtime)
			DIFF=$(echo "$END - $START" | bc)
			echo "$DIFF" >> $time_path
		done
    fi
done
