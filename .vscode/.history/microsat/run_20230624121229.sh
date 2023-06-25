#!/bin/bash

# Set the directory
dir_path="cnf_benchmarks"

# Set the program
prog="./parallel"

# Loop through all files in the directory
for file in "$dir_path"/*
do
    # Check if it is a file (not a directory)
    if [ -f "$file" ]
    then
		# Total running time of this file
		total_time=0
        # Run the program with the file as an argument
		time_taken=$(/usr/bin/time -f "%U + %S" $prog "$file" 2>&1 1>/dev/null)
		total_time=$(echo "$total_time + $time_taken" | bc -l)

		# Calculate the average time
		average_time=$(echo "scale=5; $total_time / 10" | bc -l)
		echo $average_time
		# echo "$average_time" > "${file}_average_time.txt"
    fi
done
