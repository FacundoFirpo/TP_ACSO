#!/bin/bash

# TO RUN:
# chmod +x compare_rdump.sh
# ./compare_rdump.sh <program_file1> [program_file2 ...] [--cycles=N]

set -e

# Get the absolute path of the script
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Check if program files are provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <program_file1> [program_file2 ...] [--cycles=N]"
    exit 1
fi

# Default cycles
CYCLES=4
ARGS=()

# Parse arguments
for arg in "$@"; do
    if [[ "$arg" == --cycles=* ]]; then
        CYCLES="${arg#--cycles=}"
    else
        ARGS+=("$arg")
    fi
done

# Check if the program file exists
if [ ! -f "${ARGS[0]}" ]; then
    echo "❌ Error: Program file '${ARGS[0]}' not found!"
    exit 1
fi

### 🔹 Run ref_sim_x86 and save its dumpsim
echo "Running ref_sim_x86..."
./ref_sim_x86 "${ARGS[@]}" <<EOF
run $CYCLES
rdump
q
EOF

# Ensure dumpsim exists
if [ ! -f "$SCRIPT_DIR/dumpsim" ]; then
    echo "❌ No dumpsim file found after running ref_sim_x86"
    exit 1
fi

# Copy the reference dumpsim for comparison
cp "$SCRIPT_DIR/dumpsim" "$SCRIPT_DIR/ref_dumpsim"

### 🔹 Run sim and save its dumpsim
echo "Running sim..."

cd src

# Check if sim is executable
if [ ! -x "./sim" ]; then
    echo "Making sim executable..."
    chmod +x ./sim
fi

# Try to determine the architecture of the binary
file_info=$(file ./sim)
echo "Binary info: $file_info"

# Try running with appropriate method based on architecture
if [[ "$file_info" == *"ARM"* ]]; then
    echo "Detected ARM binary, trying to run with appropriate method..."
    # If you have an ARM emulator installed, use it here
    # For example: qemu-arm ./sim "../${ARGS[@]}"
    echo "You may need to install an ARM emulator like qemu-arm to run this binary."
    echo "Or recompile the sim program for your architecture."
    exit 1
else
    # Try running normally
    ./sim "../${ARGS[@]}" <<EOF
run $CYCLES
rdump
q
EOF
fi

# Ensure dumpsim exists
if [ ! -f "$SCRIPT_DIR/src/dumpsim" ]; then
    echo "❌ No dumpsim file found after running sim"
    exit 1
fi

# Copy the sim dumpsim for comparison
cp "$SCRIPT_DIR/src/dumpsim" "$SCRIPT_DIR/sim_dumpsim"

# Go back to the main directory
cd "$SCRIPT_DIR"

### 🔹 Compare the two dumpsim files
echo "Comparing dumpsim files..."
diff -u ref_dumpsim sim_dumpsim > diff_output.txt || true

if [ -s diff_output.txt ]; then
    echo "❌ Differences found! See diff_output.txt for details."
    cat diff_output.txt
    exit 1
else
    echo "✅ No differences found in dumpsim files."
    exit 0
fi
