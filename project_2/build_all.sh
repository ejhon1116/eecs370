#!/bin/bash
# Deletes all .mc files and assembles all .as files in the current directory

# Exit immediately if a command fails
set -e

# Remove all .mc files (if any)
echo "Deleting all .mc files..."
rm -f ./tests/*.obj

# Loop over all .as files
for file in ./*.as; do
    # Skip if no .as files are found
    [ -e "$file" ] || {
        echo "No .as files found."
        exit 0
    }

    # Get the base filename (without extension)
    base="${file%.as}"

    echo "Assembling $file -> ${base}.obj"
    ./assembler "$file" "${base}.obj"
    mv ./*.obj ./tests/
done

echo "Done!"
