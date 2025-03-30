.text
// Test CBZ (Compare and Branch if Zero) instruction
MOVZ X1, #0      // Initialize X1 with 0
MOVZ X2, #10     // Initialize X2 with 10

// Branch if X1 is zero (should branch)
CBZ X1, label1

// This code should be skipped
MOVZ X3, #30     // This should be skipped

label1:
// This code should be executed after the branch
MOVZ X4, #40     // X4 = 40

// Now test when condition is not met
MOVZ X5, #50     // X5 = 50 (non-zero)

// Branch if X5 is zero (should not branch)
CBZ X5, label2

// This code should be executed (no branch)
MOVZ X6, #60     // X6 = 60

label2:
// Halt
HLT 0