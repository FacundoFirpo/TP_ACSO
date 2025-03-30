.text
// Test BGE (Branch if Greater or Equal) instruction
MOVZ X1, #20     // Initialize X1 with 20
MOVZ X2, #10     // Initialize X2 with 10

// Compare X1 and X2 (X1 > X2)
CMP X1, X2

// Branch if greater or equal (should branch)
BGE label1

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

// Branch if greater or equal (should branch)
BGE label2

// This code should be skipped
MOVZ X7, #70     // This should be skipped

label2:
// This code should be executed after the branch
MOVZ X8, #80     // X8 = 80

// Now test when condition is not met
MOVZ X9, #10     // X9 = 10
MOVZ X10, #20    // X10 = 20 (greater than X9)

// Compare X9 and X10 (X9 < X10)
CMP X9, X10

// Branch if greater or equal (should not branch)
BGE label3

// This code should be executed (no branch)
MOVZ X11, #110   // X11 = 110

label3:
// Halt
HLT 0