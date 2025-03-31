#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "shell.h"


void process_instruction() {
    // Fetch: Leer la instrucción de la memoria
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    
    // Print instruction in both hex and binary format
    printf("Executing instruction: 0x%08x (", instruction);
    for (int i = 31; i >= 0; i--) {
        printf("%d", (instruction >> i) & 1);

    }
    printf(")\n");

    uint32_t opcodes[5];
    opcodes[0] = (instruction >> 21) & 0b11111111111; // Extraer opcode21
    opcodes[1] = (instruction >> 22) & 0b1111111111;  // Extraer opcode22
    opcodes[2] = (instruction >> 23) & 0b111111111;   // Extraer opcode23
    opcodes[3] = (instruction >> 24) & 0b11111111;    // Extraer opcode24
    opcodes[4] = (instruction >> 26) & 0b111111;      // Extraer opcode26

    uint32_t opcode21 = opcodes[0];
    uint32_t opcode22 = opcodes[1];
    uint32_t opcode23 = opcodes[2];
    uint32_t opcode24 = opcodes[3];
    uint32_t opcode26 = opcodes[4];

    // Incrementar PC al siguiente instruction (cada instrucción ocupa 4 bytes)
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    // Decodificar y ejecutar
    for (int i = 0; i < 5; i++) {
        switch (opcodes[i]) {
            case 0b011010100010: // HLT
                RUN_BIT = 0;
                break;

            case 0b10101011000: // ADDS Xd, Xn, Xm (Extended Register)
            {
                uint32_t Rd = instruction & 0b11111;
                uint32_t Rn = (instruction >> 5) & 0b11111;
                uint32_t Rm = (instruction >> 16) & 0b11111;
                int64_t result = (int64_t)CURRENT_STATE.REGS[Rn] + (int64_t)CURRENT_STATE.REGS[Rm];
                NEXT_STATE.REGS[Rd] = (uint64_t)result;
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result < 0);
                break;
            }

            case 0b10110001000: // ADDS Xd, Xn, #imm (Immediate)
            {
                uint32_t Rd = instruction & 0b11111;
                uint32_t Rn = (instruction >> 5) & 0b11111;
                uint32_t imm = (instruction >> 10) & 0b111111111111;
                int64_t result = (int64_t)CURRENT_STATE.REGS[Rn] + imm;
                NEXT_STATE.REGS[Rd] = (uint64_t)result;
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result < 0);
                break;
            }

            case 0b11101011000: // SUBS Xd, Xn, Xm (Extended Register) or CMP Xn, Xm (Extended Register)
            {
                uint32_t Rd = instruction & 0b11111;
                uint32_t Rn = (instruction >> 5) & 0b11111;
                uint32_t Rm = (instruction >> 16) & 0b11111;
                uint64_t result = CURRENT_STATE.REGS[Rn] - CURRENT_STATE.REGS[Rm];
                if (Rd != 0b11111) { // If Rd is not the zero register, store the result
                    NEXT_STATE.REGS[Rd] = result;
                }
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result >> 63) & 1;
                break;
            }

            case 0b11110001000: // SUBS Xd, Xn, #imm (Immediate) or CMP Xn, #imm (Immediate)
            {
                uint32_t Rd = instruction & 0b11111;
                uint32_t Rn = (instruction >> 5) & 0b11111;
                uint32_t imm = (instruction >> 10) & 0b111111111111;
                uint64_t result = CURRENT_STATE.REGS[Rn] - imm;
                if (Rd != 0b11111) { // If Rd is not the zero register, store the result
                    NEXT_STATE.REGS[Rd] = result;
                }
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result >> 63) & 1;
                break;
            }

            case 0b11101010000: // ANDS Xd, Xn, Xm (Shifted Register)
            {
                uint32_t Rd = instruction & 0b11111;
                uint32_t Rn = (instruction >> 5) & 0b11111;
                uint32_t Rm = (instruction >> 16) & 0b11111;
                uint64_t result = CURRENT_STATE.REGS[Rn] & CURRENT_STATE.REGS[Rm];
                NEXT_STATE.REGS[Rd] = result;
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result >> 63) & 1;
                break;
            }

            case 0b11001010000: // EOR Xd, Xn, Xm (Shifted Register)
            {
                uint32_t rd = instruction & 0b11111;
                uint32_t rn = (instruction >> 5) & 0b11111;
                uint32_t rm = (instruction >> 16) & 0b11111;
             
                uint64_t result = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
                NEXT_STATE.REGS[rd] = result;
                
                break;
            }

            case 0b10101010000: // ORR Xd, Xn, Xm (Shifted Register)
            {
                uint32_t Rd = instruction & 0b11111;
                uint32_t Rn = (instruction >> 5) & 0b11111;
                uint32_t Rm = (instruction >> 16) & 0b11111;

                uint64_t result = CURRENT_STATE.REGS[Rn] | CURRENT_STATE.REGS[Rm];
                NEXT_STATE.REGS[Rd] = result;
                
                break;
            }

            case 0b000101: // B (Branch)
            {
                int32_t imm26 = instruction & 0x03FFFFFF; // Extraer bits 0-25 (26-bit immediate)
            
                // Extender el signo si el bit 25 está en 1
                if (imm26 & (1 << 25)) {
                    imm26 |= 0xFC000000; // Llenar con 1s los bits superiores
                }
            
                int32_t offset = imm26 << 2; // Multiplicar por 4
            
                NEXT_STATE.PC = CURRENT_STATE.PC + offset;
                break;
            }

            case 0b11010110000: // BR 
            {
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 9 to 5
                uint64_t target = CURRENT_STATE.REGS[rn];
                NEXT_STATE.PC = target;
                break;
            }

            case 0b01010100: // B.Cond (Conditional Branch)
            {
                // Extract the 19-bit immediate value
                int32_t imm19 = (instruction >> 5) & 0b1111111111111111111;
                
                // Sign extend the 19-bit value first
                if (imm19 & (1 << 18)) { // Check if bit 18 (sign bit of imm19) is set
                    imm19 |= 0xFFF80000; // Set bits 19-31 to 1 for sign extension
                }
                
                // Then multiply by 4 to get byte offset
                int32_t offset = imm19 << 2;
                
                uint32_t cond = instruction & 0b1111; // Extract condition code (bits 0-3)
                
                int branch = 0;
                switch (cond) {
                    case 0b0000: // BEQ (Branch if Equal)
                        if (CURRENT_STATE.FLAG_Z) {
                            branch = 1;
                        }
                        break;
                    case 0b0001: // BNE (Branch if Not Equal)
                        if (!CURRENT_STATE.FLAG_Z) {
                            branch = 1;
                        }
                        break;
                    case 0b1010: // BGE (Branch if Greater or Equal)
                        if (!CURRENT_STATE.FLAG_N) { // FLAG_V is always 0
                            branch = 1;
                        }
                        break;
                    case 0b1011: // BLT (Branch if Less Than)
                        if (CURRENT_STATE.FLAG_N) { // FLAG_V is always 0
                            branch = 1;
                        }
                        break;
                    case 0b1100: // BGT (Branch if Greater Than)
                        if (!CURRENT_STATE.FLAG_Z && !CURRENT_STATE.FLAG_N) { // FLAG_V is always 0
                            branch = 1;
                        }
                        break;
                    case 0b1101: // BLE (Branch if Less or Equal)
                        if (CURRENT_STATE.FLAG_Z || CURRENT_STATE.FLAG_N) { // FLAG_V is always 0
                            branch = 1;
                        }
                        break;
                    default:
                        break;
                }
                
                if (branch) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
                }
                // If branch not taken, PC+4 is already set above
                break;
            }

            case 0b1101001101: // LSL (immediate) or LSR (immediate)
            {
                uint32_t rd = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                uint32_t imms = (instruction >> 10) & 0b111111; // Extract bits 10 to 15
                uint32_t immr = (instruction >> 16) & 0b111111; // Extract bits 16 to 21

                uint64_t result = 0;

                if (imms==0b11111){ // LSR
                    uint32_t shift = immr; // Calculate the shift amount
                    result = CURRENT_STATE.REGS[rn] >> shift;
                } 
                else { // LSL
                    uint32_t shift = 64 - immr; // Calculate the shift amount
                    result = CURRENT_STATE.REGS[rn] << shift;
                }
                                
                NEXT_STATE.REGS[rd] = result;
                break;
            }

            case 0b11111000000: // STUR Xn, [Xn, #imm]
            {
                uint32_t Rt = instruction & 0b11111;             // Bits 0-4: Rt (Registro a almacenar)
                uint32_t Rn = (instruction >> 5) & 0b11111;      // Bits 5-9: Rn (Registro base)
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Bits 12-20: desplazamiento inmediato (9 bits)
                // Sign extend imm9 if negative
                if (imm9 & (1 << 8)) { // Check if bit 8 (sign bit) is 1
                    imm9 |= 0xFFFFFF00; // Extend sign by setting bits 8-31 to 1
                }
                // Determinar la dirección base
                uint64_t address = CURRENT_STATE.REGS[Rn] + imm9;
                
                // Read existing memory value
                uint32_t existing_data = mem_read_32(address & ~0x3); // Align to word boundary
                
                // Calculate byte position within the word (0, 1, 2, or 3)
                int byte_pos = address & 0x3;
                
                // Extract the byte from Rt
                uint8_t byte_to_store = CURRENT_STATE.REGS[Rt] & 0xFF;
                
                // Create a mask and position the byte
                uint32_t byte_mask = 0xFF << (byte_pos * 8);
                uint32_t positioned_byte = byte_to_store << (byte_pos * 8);
                
                // Update only the specific byte in the word
                uint32_t new_data = (existing_data & ~byte_mask) | positioned_byte;
                
                // Write back to memory
                mem_write_32(address & ~0x3, new_data);
                
                break;
            }
            
            case 0b00111000000: // STURB Xn, [Xn, #imm]
            {
                uint32_t Rt = instruction & 0b11111;             // Bits 0-4: Rt (Registro a almacenar)
                uint32_t Rn = (instruction >> 5) & 0b11111;      // Bits 5-9: Rn (Registro base)
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Bits 12-20: desplazamiento inmediato (9 bits)
                // Sign extend imm9 if negative
                if (imm9 & (1 << 8)) { // Check if bit 8 (sign bit) is 1
                    imm9 |= 0xFFFFFF00; // Extend sign by setting bits 8-31 to 1
                }
                // Determinar la dirección base
                uint64_t address = CURRENT_STATE.REGS[Rn] + imm9;
                
                // Read existing memory value
                uint32_t existing_data = mem_read_32(address & ~0x3); // Align to word boundary
                
                // Calculate byte position within the word (0, 1, 2, or 3)
                int byte_pos = address & 0x3;
                
                // Extract the byte from Rt
                uint8_t byte_to_store = CURRENT_STATE.REGS[Rt] & 0xFF;
                
                // Create a mask and position the byte
                uint32_t byte_mask = 0xFF << (byte_pos * 8);
                uint32_t positioned_byte = byte_to_store << (byte_pos * 8);
                
                // Update only the specific byte in the word
                uint32_t new_data = (existing_data & ~byte_mask) | positioned_byte;
                
                // Write back to memory
                mem_write_32(address & ~0x3, new_data);
                
                break;
            }
            
            case 0b0011100001: // LDURB
            {
                uint32_t rt = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Extract bits 12 to 20
                
                // Sign extend imm9 if negative
                if (imm9 & (1 << 8)) { // Check if bit 8 (sign bit) is 1
                    imm9 |= 0xFFFFFF00; // Extend sign by setting bits 8-31 to 1
                }
                
                uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
                
                // Read the word containing the byte
                uint32_t data = mem_read_32(address & ~0x3); // Align to word boundary
                
                // Calculate byte position within the word (0, 1, 2, or 3)
                int byte_pos = address & 0x3;
                
                // Extract the specific byte
                int8_t byte = (data >> (byte_pos * 8)) & 0xFF;
                
                // Sign extend to 64 bits
                NEXT_STATE.REGS[rt] = (uint64_t)(int64_t)byte;
                
                break;
            }
            
            case 0b0111100001: // LDURH
            {
                uint32_t rt = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Extract bits 12 to 20
                
                // Sign extend imm9 if negative
                if (imm9 & (1 << 8)) { // Check if bit 8 (sign bit) is 1
                    imm9 |= 0xFFFFFF00; // Extend sign by setting bits 8-31 to 1
                }
                
                uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
                
                // Read the word containing the halfword
                uint32_t data = mem_read_32(address & ~0x3); // Align to word boundary
                
                // Calculate halfword position within the word (0 or 2)
                int halfword_pos = (address & 0x3) / 2;
                
                // Extract the specific halfword
                int16_t halfword = (data >> (halfword_pos * 16)) & 0xFFFF;
                
                // Sign extend to 64 bits
                NEXT_STATE.REGS[rt] = (uint64_t)(int64_t)halfword;
                
                break;
            }

            case 0b0111100001: // LDURH
            {
                uint32_t rt = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Extract bits 12 to 20

                uint64_t address = CURRENT_STATE.REGS[rn] + imm9;

                uint32_t data = mem_read_32(address);
                uint16_t halfword = data & 0xFFFF; // Extract the lowest 16 bits
                NEXT_STATE.REGS[rt] = halfword;
                
                break;
            }
            

            case 0b0011100001: // LDURB
            {
                uint32_t rt = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Extract bits 12 to 20
                
                // Sign extend imm9 if negative
                if (imm9 & (1 << 8)) { // Check if bit 8 (sign bit) is 1
                    imm9 |= 0xFFFFFF00; // Extend sign by setting bits 8-31 to 1
                }
                
                uint64_t address = CURRENT_STATE.REGS[rn] + imm9;

                uint32_t data = mem_read_32(address);
                uint8_t byte = data & 0xFF; // Extract the lowest 8 bits
                NEXT_STATE.REGS[rt] = byte;
                
                break;
            }
             
            
            case 0b11010010100: // MOVZ Xd, #imm16
            {
                uint32_t rd = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t imm16 = (instruction >> 5) & 0b1111111111111111; // Extract bits 5 to 20

                NEXT_STATE.REGS[rd] = imm16;

                break;    
            }

            case 0b10001011000: // ADD Xd, Xn, Xm (Extended Register)
            {
                uint32_t rd = instruction & 0b11111; // Bits 0-4
                uint32_t rn = (instruction >> 5) & 0b11111; // Bits 5-9
                uint32_t shift = (instruction >> 10) & 0b111; // Bits 10-12
                uint32_t rm = (instruction >> 16) & 0b11111; // Bits 16-20
                uint64_t sum = CURRENT_STATE.REGS[rm];
            
                if (shift == 1) {
                    sum = sum << 12;
                }
            
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + sum;
                break;
            }
            

            case 0b10010001: // ADD Xd, Xn, #imm (Immediate)
            {
                uint32_t rd = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                uint32_t imm12 = (instruction >> 10) & 0b111111111111; // Extract bits 10 to 21
                uint32_t shift = (instruction >> 22) & 0b11; // Extract bits 22 to 23
                if (shift == 0b00) {
                    imm12 = (uint64_t) imm12;
                } else if (shift == 0b01) {
                    imm12 = (uint64_t) imm12 << 12;
                }

                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + imm12;

                // Update flags for ADD operation
                NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[rd] == 0);
                NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[rd] >> 63) & 1;
                break;
            }

            case 0b10011011000: // MUL Xn, Xm, Xs
            {
                uint32_t rd = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                uint32_t rm = (instruction >> 16) & 0b11111; // Extract bits 16 to 20
            
                uint64_t result = CURRENT_STATE.REGS[rn] * CURRENT_STATE.REGS[rm]; // Eliminar la suma de 32
                NEXT_STATE.REGS[rd] = result;
            
                break;
            }

            case 0b10110100: // CBZ Xn, #imm
            {
                uint32_t rn = instruction & 0b11111; // Extract bits 0 to 4
                int32_t imm19 = (instruction >> 5) & 0x7FFFF; // Extract bits 5 to 23 (19-bit immediate)
            
                // Extender el signo manualmente si el bit 18 está en 1
                if (imm19 & (1 << 18)) {
                    imm19 |= 0xFFF80000; // Extender con 1s los bits superiores
                }
            
                int32_t offset = imm19 << 2; // Multiplicar por 4
            
                if (CURRENT_STATE.REGS[rn] == 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
                }
                break;
            }

            case 0b10110101: // CBNZ Xn, #imm
            {
                uint32_t rn = instruction & 0b11111; // Extraer bits 0 a 4
                int32_t imm19 = (instruction >> 5) & 0x7FFFF; // Extraer bits 5 a 23 (19-bit immediate)
            
                // Extender el signo si el bit 18 está en 1
                if (imm19 & (1 << 18)) {
                    imm19 |= 0xFFF80000; // Llenar con 1s los bits superiores
                }
            
                int32_t offset = imm19 << 2; // Multiplicar por 4
            
                if (CURRENT_STATE.REGS[rn] != 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
                }
                break;
            }




            default:
                break;
        }
    }
}
