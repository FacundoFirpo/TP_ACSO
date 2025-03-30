.text
// Test BGT (Branch if Greater Than) instruction
MOVZ X1, #20     // Initialize X1 with 20
MOVZ X2, #10     // Initialize X2 with 10

// Compare X1 and X2 (X1 > X2)
CMP X1, X2

// Branch if greater than (should branch)
BGT label1

// This code should be skipped
MOVZ X3, #30     // This should be skipped

label1:
// This code should be executed after the branch
MOVZ X4, #40     // X4 = 40

// Now test with equal values
MOVZ X5, #50     // X5 = 50
MOVZ X6, #50     // X6 = 50 (same as X5)

// Compare X5 and X6 (they are equal)
CMP X5, X6

// Branch if greater than (should not branch)
BGT label2

// This code should be executed (no branch)
MOVZ X7, #70     // X7 = 70

label2:
// Now test when condition is not met
MOVZ X8, #10     // X8 = 10
MOVZ X9, #20     // X9 = 20 (greater than X8)

// Compare X8 and X9 (X8 < X9)
CMP X8, X9

// Branch if greater than (should not branch)
BGT label3

// This code should be executed (no branch)
MOVZ X10, #100   // X10 = 100

label3:
// Halt
HLT 0