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
                uint64_t result = CURRENT_STATE.REGS[Rn] + CURRENT_STATE.REGS[Rm];
                NEXT_STATE.REGS[Rd] = result;
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result >> 63) & 1;
                break;
            }

            case 0b10110001000: // ADDS Xd, Xn, #imm (Immediate)
            {
                uint32_t Rd = instruction & 0b11111;
                uint32_t Rn = (instruction >> 5) & 0b11111;
                uint32_t imm = (instruction >> 10) & 0b111111111111;
                uint64_t result = CURRENT_STATE.REGS[Rn] + imm;
                NEXT_STATE.REGS[Rd] = result;
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result >> 63) & 1;
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
                int32_t imm26 = (instruction) & 0b11111111111111111111111111; // Extract bits 0 to 25
                if (imm26 & (1 << 25)) { // Check bit 25 for sign extension
                    imm26 |= 0xFC000000; // Extend sign by setting bits 26-31 to 1
                }
                NEXT_STATE.PC = CURRENT_STATE.PC + imm26;
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
                int32_t imm19 = (instruction >> 5) & 0b1111111111111111111; // Extraer 19 bits del inmediato
                int32_t imm21 = (imm19 << 2); // Extender a 21 bits
                if (imm19 & (1 << 18)) { // Extender el signo
                    imm21 |= 0b11111111111000000000000000000000;
                }
                uint32_t cond = (instruction >> 0) & 0b1111; // Extraer condición correctamente

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
                    NEXT_STATE.PC = CURRENT_STATE.PC + imm21;
                }
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
                    uint32_t shift = immr & 0b111111; // Calculate the shift amount
                    uint64_t result = CURRENT_STATE.REGS[rn] >> shift;
                } 
                else { // LSL
                    uint32_t shift = 64 - immr & 0b111111; // Calculate the shift amount
                    uint64_t result = CURRENT_STATE.REGS[rn] << shift;
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

                // Guardar el valor de X[Rt] en la memoria
                uint32_t data = CURRENT_STATE.REGS[Rt];
                mem_write_32(address, data);

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
                // Guardar el valor de X[Rt] en la memoria
                uint8_t data = CURRENT_STATE.REGS[Rt];
                
                mem_write_32(address, (uint32_t)data);
                break;
            }

            case 0b01111000000: // STURH Xn, [Xn, #imm]
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
                // Guardar el valor de X[Rt] en la memoria
                uint16_t data = CURRENT_STATE.REGS[Rt];
                
                mem_write_32(address, (uint32_t)data);
                break;
            }
                

            case 0b1111100001: // LDUR
            {
                uint32_t rt = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Extract bits 12 to 20
                
                uint64_t address = CURRENT_STATE.REGS[rn];
                address += imm9;

                NEXT_STATE.REGS[rt] = mem_read_32(address);
                break;
            }

            case 0b0111100001: // LDURH
            {
                uint32_t rt = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Extract bits 12 to 20

                uint64_t address = CURRENT_STATE.REGS[rn];
                address += imm9;

                uint32_t data = mem_read_32(address);
                data = (data >> 16) & 0xFFFF; // Extraer los 16 bits superiores
                NEXT_STATE.REGS[rt] = data;
                
                break;
            }
            

            case 0b0011100001: // LDURB
            {
                uint32_t rt = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                int32_t imm9 = (instruction >> 12) & 0b111111111; // Extract bits 12 to 20
                
                uint64_t address = CURRENT_STATE.REGS[rn];
                address += imm9;

                uint32_t data = mem_read_32(address);
                data = (data >> 24) & 0xFF; // Extraer el byte superior
                NEXT_STATE.REGS[rt] = data;
                
                break;
            }
             
            
            case 0b11010010100: // MOVZ Xd, #imm16
            {
                uint32_t rd = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t imm16 = (instruction >> 5) & 0b1111111111111111; // Extract bits 5 to 20

                NEXT_STATE.REGS[rd] = imm16;

                break;    
            }

            case 0b10001011001: // ADD Xd, Xn, Xm (Extended Register)
            {
                uint32_t rd = instruction & 0b11111; // Extract bits 0 to 4
                uint32_t rn = (instruction >> 5) & 0b11111; // Extract bits 5 to 9
                uint32_t shift = (instruction >> 10) & 0b111; // Extract bits 10 to 12
                uint32_t rm = (instruction >> 16) & 0b11111; // Extract bits 16 to 20
                
                uint32_t sum = CURRENT_STATE.REGS[rm];
                if (shift != 0) {
                    sum = CURRENT_STATE.REGS[rm] << shift;
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

                uint64_t result = 32 + (CURRENT_STATE.REGS[rn] * CURRENT_STATE.REGS[rm]);
                NEXT_STATE.REGS[rd] = result;

                // Update flags for MUL operation
                NEXT_STATE.FLAG_Z = (result == 0);
                NEXT_STATE.FLAG_N = (result >> 63) & 1;

                break;
            }






            default:
                break;
        }
    }
}