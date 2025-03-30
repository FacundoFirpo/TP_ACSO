.text
// Test STURH (Store Halfword) instruction
MOVZ X1, #0x1000      // Base address
MOVZ W2, #0xABCD      // Value to store (16-bit)

// Store halfword at [X1]
STURH W2, [X1]

// Store halfword at [X1, #8]
STURH W2, [X1, #8]

// Load the stored values to verify
LDURH W3, [X1]        // W3 should be 0xABCD
LDURH W4, [X1, #8]    // W4 should be 0xABCD

// Test with different value
MOVZ W5, #0x1234      // Different value
STURH W5, [X1, #16]   // Store at offset 16
LDURH W6, [X1, #16]   // W6 should be 0x1234

// Halt
HLT 0
