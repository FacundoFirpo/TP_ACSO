.text
// Test STURH (Store Halfword) instruction
MOVZ X1, #0x1000     // Base address
MOVZ X2, #0xABCD     // Value to store (16-bit)

// Store halfword at [X1]
STURH X2, [X1]

// Store halfword at [X1, #8]
STURH X2, [X1, #8]

// Load the stored values to verify
LDURH X3, [X1]       // X3 should be 0xABCD
LDURH X4, [X1, #8]   // X4 should be 0xABCD

// Test with different value
MOVZ X5, #0x1234     // Different value
STURH X5, [X1, #16]  // Store at offset 16
LDURH X6, [X1, #16]  // X6 should be 0x1234

// Halt
HLT 0