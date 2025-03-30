.text
// Test B (Branch) instruction
MOVZ X1, #10     // Initialize X1 with 10
MOVZ X2, #20     // Initialize X2 with 20

// Branch forward to label1
B label1

// This code should be skipped
MOVZ X3, #30     // This should be skipped
MOVZ X4, #40     // This should be skipped

label1:
// This code should be executed after the branch
MOVZ X5, #50     // X5 = 50
MOVZ X6, #60     // X6 = 60

// Halt
HLT 0