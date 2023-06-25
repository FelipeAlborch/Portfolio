#!/bin/bash

# Check if a byte count and file name are provided as arguments
if [ $# -ne 2 ]; then
	  echo "Please provide a byte count and file name as arguments."
	    exit 1
fi

# Store the byte count and file name from the arguments
byte_count=$1
filename=$2

# Display the bits of the file in real time with the specified byte count per row
watch -n 0.2 -x bash -c "xxd -b -c$byte_count $filename | cut -d' ' -f2-"

