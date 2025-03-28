// Test for ADDS instruction
.global _start
_start:
    // Initialize registers with test values
    MOVZ X1, #10, LSL #0    // X1 = 10
    MOVZ X2, #20, LSL #0    // X2 = 20
    
    // Test ADDS with registers
    ADDS X0, X1, X2         // X0 = X1 + X2 = 30
    
    // Test ADDS with immediate
    ADDS X3, X1, #5         // X3 = X1 + 5 = 15
    
    // Test setting flags (Zero flag)
    MOVZ X4, #0, LSL #0     // X4 = 0
    ADDS X5, X4, X4         // X5 = 0, sets Z flag
    
    // Test setting flags (Negative flag)
    MOVZ X6, #0xFFFF, LSL #48  // X6 = 0xFFFF000000000000
    ADDS X7, X6, #1         // X7 = X6 + 1, sets N flag
    
    // Halt
    HLT