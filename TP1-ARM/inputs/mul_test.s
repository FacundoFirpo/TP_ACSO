.text
// Test MUL instruction
MOVZ X1, #10     // Initialize X1 with 10
MOVZ X2, #20     // Initialize X2 with 20

// Multiply registers
MUL X3, X1, X2   // X3 = X1 * X2 = 200

// Test with different values
MOVZ X4, #5      // Initialize X4 with 5
MOVZ X5, #6      // Initialize X5 with 6
MUL X6, X4, X5   // X6 = X4 * X5 = 30

// Test with zero
MOVZ X7, #0      // Initialize X7 with 0
MUL X8, X1, X7   // X8 = X1 * X7 = 0

// Halt
HLT 0