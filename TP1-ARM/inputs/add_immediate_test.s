.text
// Test ADD (Immediate) instruction
MOVZ X1, #10     // Initialize X1 with 10

// Add immediate value
ADD X2, X1, #20  // X2 = X1 + 20 = 30

// Test with different register
MOVZ X3, #100    // Initialize X3 with 100
ADD X4, X3, #50  // X4 = X3 + 50 = 150

// Test with shifted immediate
MOVZ X5, #1      // Initialize X5 with 1
ADD X6, X5, #1, LSL #12  // X6 = X5 + (1 << 12) = 1 + 4096 = 4097

// Halt
HLT 0