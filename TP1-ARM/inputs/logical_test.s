.text
// Initialize registers with test values
MOVZ X1, #0xAA, LSL #0  // X1 = 0xAA (10101010)
MOVZ X2, #0x55, LSL #0  // X2 = 0x55 (01010101)

// Test ANDS
ANDS X3, X1, X2         // X3 = X1 & X2 = 0x00

// Test EOR
EOR X4, X1, X2          // X4 = X1 ^ X2 = 0xFF

// Test ORR
ORR X5, X1, X2          // X5 = X1 | X2 = 0xFF

// Test with different values
MOVZ X6, #0xF0, LSL #0  // X6 = 0xF0 (11110000)
MOVZ X7, #0x0F, LSL #0  // X7 = 0x0F (00001111)

ANDS X8, X6, X7         // X8 = X6 & X7 = 0x00
EOR X9, X6, X7          // X9 = X6 ^ X7 = 0xFF
ORR X10, X6, X7         // X10 = X6 | X7 = 0xFF

// Halt
HLT