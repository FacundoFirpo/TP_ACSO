.text
// Test MOVZ with different shift values
MOVZ X0, #0xABCD, LSL #0   // X0 = 0xABCD
MOVZ X1, #0xABCD, LSL #16  // X1 = 0xABCD0000
MOVZ X2, #0xABCD, LSL #32  // X2 = 0xABCD00000000
MOVZ X3, #0xABCD, LSL #48  // X3 = 0xABCD000000000000

// Test overwriting values
MOVZ X4, #0xFFFF, LSL #0   // X4 = 0xFFFF
MOVZ X4, #0xAAAA, LSL #16  // X4 = 0xAAAA0000

// Test with zero value
MOVZ X5, #0, LSL #0        // X5 = 0

// Test with maximum 16-bit value
MOVZ X6, #0xFFFF, LSL #0   // X6 = 0xFFFF

// Halt
HLT 0