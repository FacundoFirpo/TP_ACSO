// Test for shift operations (LSL, LSR)
.global _start
_start:
    // Initialize registers with test values
    MOVZ X1, #1, LSL #0     // X1 = 1
    
    // Test LSL
    LSL X2, X1, #4          // X2 = X1 << 4 = 16
    LSL X3, X1, #8          // X3 = X1 << 8 = 256
    
    // Test with different values
    MOVZ X4, #0xF, LSL #0   // X4 = 0xF
    LSL X5, X4, #4          // X5 = X4 << 4 = 0xF0
    
    // Test LSR
    MOVZ X6, #0x100, LSL #0 // X6 = 0x100
    LSR X7, X6, #4          // X7 = X6 >> 4 = 0x10
    LSR X8, X6, #8          // X8 = X6 >> 8 = 0x1
    
    // Test with different values
    MOVZ X9, #0xF0, LSL #0  // X9 = 0xF0
    LSR X10, X9, #4         // X10 = X9 >> 4 = 0xF
    
    // Halt
    HLT