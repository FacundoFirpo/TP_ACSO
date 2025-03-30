.text
// Test ADD (Register) instruction
MOVZ X1, #10     // Initialize X1 with 10
MOVZ X2, #20     // Initialize X2 with 20

// Add registers without shift
ADD X3, X1, X2   // X3 = X1 + X2 = 30

// Test with shift
MOVZ X4, #1      // Initialize X4 with 1
ADD X5, X1, X4, LSL #2  // X5 = X1 + (X4 << 2) = 10 + 4 = 14

// Test with different values
MOVZ X6, #100    // Initialize X6 with 100
MOVZ X7, #200    // Initialize X7 with 200
ADD X8, X6, X7   // X8 = X6 + X7 = 300

// Halt
HLT 0