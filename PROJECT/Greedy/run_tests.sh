#!/bin/bash

# Clear previous build artifacts
echo "[INFO] Cleaning project..."
make clean

# Compile the project
echo "[INFO] Building project..."
make

# Folder containing the instances
INSTANCE_FOLDER="./Instances"

# Binary name
EXECUTABLE="./ra"

# Check if compilation succeeded
if [ ! -f "$EXECUTABLE" ]; then
    echo "[ERROR] Compilation failed or ra executable not found."
    exit 1
fi

echo ""
echo "========== Starting Referee Assignment Tests =========="

# Loop through each RA-*.txt file in the Instances folder
for instance in "$INSTANCE_FOLDER"/RA-*.txt; do
    echo ""
    echo ">>> Running test for: $(basename "$instance")"
    $EXECUTABLE "$instance"
    echo "------------------------------------------------------"
done

echo ""
echo "✅ All tests completed."
