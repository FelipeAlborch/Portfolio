#!/bin/bash

# Check if a file name is provided as an argument
if [ $# -eq 0 ]; then
  echo "Please provide a file name as an argument."
  exit 1
fi

# Store the file name from the argument
filename=$1

# Display the bits of the file in real time
watch -n 0.2 -x bash -c "xxd -b $filename | cut -d' ' -f2-"