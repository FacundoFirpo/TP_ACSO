.text
// Test BR (Branch to Register) instruction
MOVZ X1, #10     // Initialize X1 with 10
MOVZ X2, #20     // Initialize X2 with 20

// Branch to the address in X10
BR target:

// This code should be skipped
MOVZ X2, #20     // This should be skipped
MOVZ X3, #30     // This should be skipped

target:
    // This code should be executed after the branch
    MOVZ X4, #40     // X4 = 40
    MOVZ X5, #50     // X5 = 50

    // Halt
    HLT 0