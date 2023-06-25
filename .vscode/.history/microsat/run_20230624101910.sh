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
        # Run the program with the file as an argument
        $prog "$file"
    fi
done
