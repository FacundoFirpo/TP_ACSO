.text
// Test BR (Branch to Register) instruction
MOVZ X1, #10     // Initialize X1 with 10
MOVZ X2, #20     // Initialize X2 with 20

// Store the target address in X10 using MOVZ
MOVZ X10, #0x1000  // Assuming code is loaded at address 0x1000
ADD X10, X10, #32  // Adjust to point to the target label (approximate offset)

// Branch to the address in X10
BR X10

// This code should be skipped
MOVZ X2, #20     // This should be skipped
MOVZ X3, #30     // This should be skipped

target:
    // This code should be executed after the branch
    MOVZ X4, #40     // X4 = 40
    MOVZ X5, #50     // X5 = 50

    // Halt
    HLT 0