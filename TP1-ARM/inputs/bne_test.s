.text
// Test BNE (Branch if Not Equal) instruction
MOVZ X1, #10     // Initialize X1 with 10
MOVZ X2, #20     // Initialize X2 with 20

// Compare X1 and X2 (they are not equal)
CMP X1, X2

// Branch if not equal (should branch)
BNE label1

// This code should be skipped
MOVZ X3, #30     // This should be skipped

label1:
// This code should be executed after the branch
MOVZ X4, #40     // X4 = 40

// Now test when condition is not met
MOVZ X5, #50     // X5 = 50
MOVZ X6, #50     // X6 = 50 (same as X5)

// Compare X5 and X6 (they are equal)
CMP X5, X6

// Branch if not equal (should not branch)
BNE label2

// This code should be executed (no branch)
MOVZ X7, #70     // X7 = 70

label2:
// Halt
HLT 0