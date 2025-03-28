// Test for memory operations (LDUR, LDURH)
.global _start
_start:
    // Initialize registers
    MOVZ X1, #data, LSL #0  // X1 = address of data
    
    // Test LDUR
    LDUR X2, [X1, #0]       // Load word from data
    LDUR X3, [X1, #8]       // Load word from data+8
    
    // Test LDURH
    LDURH W4, [X1, #0]      // Load halfword from data
    LDURH W5, [X1, #2]      // Load halfword from data+2
    
    // Test with offset
    MOVZ X6, #data, LSL #0  // X6 = address of data
    LDUR X7, [X6, #16]      // Load word from data+16
    
    // Halt
    HLT

// Data section
.data
data:
    .word 0x12345678
    .word 0x9ABCDEF0
    .word 0x11223344
    .word 0x55667788
    .word 0xAABBCCDD