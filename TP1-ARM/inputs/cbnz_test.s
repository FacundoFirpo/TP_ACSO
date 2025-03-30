.text
// Test CBNZ (Compare and Branch if Not Zero) instruction
MOVZ X1, #10     // Initialize X1 with 10 (non-zero)
MOVZ X2, #0      // Initialize X2 with 0

// Branch if X1 is not zero (should branch)
CBNZ X1, label1

// This code should be skipped
MOVZ X3, #30     // This should be skipped

label1:
// This code should be executed after the branch
MOVZ X4, #40     // X4 = 40

// Now test when condition is not met
// Branch if X2 is not zero (should not branch)
CBNZ X2, label2

// This code should be executed (no branch)
MOVZ X5, #50     // X5 = 50

label2:
// Halt
HLT 0