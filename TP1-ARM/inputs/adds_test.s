.text
// Initialize registers with test values
MOVZ X1, #10    // X1 = 10
MOVZ X2, #20    // X2 = 20

// Test ADDS with registers
ADDS X0, X1, X2         // X0 = X1 + X2 = 30

// Test ADDS with immediate
ADDS X3, X1, #5         // X3 = X1 + 5 = 15

// Test setting flags (Zero flag)
MOVZ X4, #0     // X4 = 0
ADDS X5, X4, X4         // X5 = 0, sets Z flag

// Halt
HLT 0
