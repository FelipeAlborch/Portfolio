#!/bin/bash

# Dimensions of the bitmap
WIDTH=10
HEIGHT=16

# Refresh rate in seconds
REFRESH_RATE=0.2

# Path to the bitmap file
BITMAP_FILE="bitmap-file.bin"

# Clear the terminal
clear

# Function to set the background color of a character cell
set_bg_color() {
  if [[ "$1" == "0" ]]; then
    tput setab 2  # Green color for unset bits
  else
    tput setab 4  # Blue color for set bits
  fi
}

# Get the file size in bytes
file_size=$(wc -c < "$BITMAP_FILE")

# Calculate the total number of bits
total_bits=$((file_size * 8))

# Loop through the bits and display them as a color matrix
index=0
while true; do
  # Clear the terminal
  tput cup 0 0

  # Display the color matrix
  for ((y = 0; y < HEIGHT; y++)); do
    for ((x = 0; x < WIDTH; x++)); do
      bit_index=$((index % total_bits))

      # Read the corresponding byte from the file
      byte_offset=$((bit_index / 8))
      bit_offset=$((bit_index % 8))
      hex_value=$(dd if="$BITMAP_FILE" bs=1 count=1 skip="$byte_offset" 2>/dev/null | xxd -p)
      bit_value=$(echo "ibase=16; obase=2; ${hex_value:-0} + 0" | bc | rev | cut -c "$((bit_offset + 1))")

      set_bg_color "$bit_value"
      echo -n " "
      index=$((index + 1))
    done
    echo
  done

  # Display the bit count at the bottom
  tput cup $((HEIGHT + 1)) 0
  echo "Total Bits: $total_bits"

  # Wait for the specified refresh rate
  sleep "$REFRESH_RATE"
done
