.text
MOVZ X1, #30, LSL #0    // X1 = 30
MOVZ X2, #20, LSL #0    // X2 = 20

// Test SUBS with registers
SUBS X0, X1, X2         // X0 = X1 - X2 = 10

// Test SUBS with immediate
SUBS X3, X1, #5         // X3 = X1 - 5 = 25

// Test CMP (SUBS with XZR as destination)
CMP X1, X2              // Compare X1 and X2, sets flags

// Test setting flags (Zero flag)
MOVZ X4, #15, LSL #0    // X4 = 15
SUBS X5, X4, X4         // X5 = 0, sets Z flag

// Test setting flags (Negative flag)
MOVZ X6, #10, LSL #0    // X6 = 10
MOVZ X7, #20, LSL #0    // X7 = 20
SUBS X8, X6, X7         // X8 = X6 - X7 = -10, sets N flag
    
// Halt
HLT 0