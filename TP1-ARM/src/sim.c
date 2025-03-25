#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "shell.h"


void process_instruction() {
    // Fetch: Leer la instrucción de la memoria
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    printf("Executing instruction: 0x%08X\n", instruction);

    uint32_t opcode21 = (instruction >> 21) & 0x7FF; // Extraer opcode
    uint32_t opcode22 = (instruction >> 22) & 0x7FF; // Extraer opcode
    uint32_t opcode26 = (instruction >> 26) & 0x3F; // Extraer opcode
    uint32_t opcode24 = (instruction >> 24) & 0x3F; // Extraer opcode

    // Incrementar PC al siguiente instruction (cada instrucción ocupa 4 bytes)
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    // Decodificar y ejecutar
    switch (opcode26) {
        case 0x6A2: // HLT
            RUN_BIT = 0;
            break;

        case 0x558: // ADDS Xd, Xn, Xm (Extended Register)
        {
            uint32_t Rd = instruction & 0x1F;
            uint32_t Rn = (instruction >> 5) & 0x1F;
            uint32_t Rm = (instruction >> 16) & 0x1F;
            uint64_t result = CURRENT_STATE.REGS[Rn] + CURRENT_STATE.REGS[Rm];
            NEXT_STATE.REGS[Rd] = result;
            NEXT_STATE.FLAG_Z = (result == 0);
            NEXT_STATE.FLAG_N = (result >> 63) & 1;
            break;
        }

        case 0x588: // ADDS Xd, Xn, #imm (Immediate)
        {
            uint32_t Rd = instruction & 0x1F;
            uint32_t Rn = (instruction >> 5) & 0x1F;
            uint32_t imm = (instruction >> 10) & 0xFFF;
            uint64_t result = CURRENT_STATE.REGS[Rn] + imm;
            NEXT_STATE.REGS[Rd] = result;
            NEXT_STATE.FLAG_Z = (result == 0);
            NEXT_STATE.FLAG_N = (result >> 63) & 1;
            break;
        }

        case 0x758: // SUBS Xd, Xn, Xm (Extended Register) or CMP Xn, Xm (Extended Register)
        {
            uint32_t Rd = instruction & 0x1F;
            uint32_t Rn = (instruction >> 5) & 0x1F;
            uint32_t Rm = (instruction >> 16) & 0x1F;
            uint64_t result = CURRENT_STATE.REGS[Rn] - CURRENT_STATE.REGS[Rm];
            if (Rd != 0x1F) { // If Rd is not the zero register, store the result
                NEXT_STATE.REGS[Rd] = result;
            }            NEXT_STATE.FLAG_Z = (result == 0);
            NEXT_STATE.FLAG_N = (result >> 63) & 1;
            break;
        }

        case 0x788: // SUBS Xd, Xn, #imm (Immediate) or CMP Xn, #imm (Immediate)
        {
            uint32_t Rd = instruction & 0x1F;
            uint32_t Rn = (instruction >> 5) & 0x1F;
            uint32_t imm = (instruction >> 10) & 0xFFF;
            uint64_t result = CURRENT_STATE.REGS[Rn] - imm;
            if (Rd != 0x1F) { // If Rd is not the zero register, store the result
                NEXT_STATE.REGS[Rd] = result;
            }            NEXT_STATE.FLAG_Z = (result == 0);
            NEXT_STATE.FLAG_N = (result >> 63) & 1;
            break;
        }

        case 0x750: // ANDS Xd, Xn, Xm (Shifted Register)
        {
            uint32_t Rd = instruction & 0x1F;
            uint32_t Rn = (instruction >> 5) & 0x1F;
            uint32_t Rm = (instruction >> 16) & 0x1F;
            uint64_t result = CURRENT_STATE.REGS[Rn] & CURRENT_STATE.REGS[Rm];
            NEXT_STATE.REGS[Rd] = result;
            NEXT_STATE.FLAG_Z = (result == 0);
            NEXT_STATE.FLAG_N = (result >> 63) & 1;
            break;
        }

        case 0x650: // EOR Xd, Xn, Xm (Shifted Register)
        {
            uint32_t Rd = instruction & 0x1F;
            uint32_t Rn = (instruction >> 5) & 0x1F;
            uint32_t Rm = (instruction >> 16) & 0x1F;
            uint64_t result = CURRENT_STATE.REGS[Rn] ^ CURRENT_STATE.REGS[Rm];
            NEXT_STATE.REGS[Rd] = result;
            NEXT_STATE.FLAG_Z = (result == 0);
            NEXT_STATE.FLAG_N = (result >> 63) & 1;
            break;
        }

        case 0x2A0: // beq
        {
            if (CURRENT_STATE.FLAG_Z == 1) {
                uint32_t imm = instruction & 0x3FFFFFF;
                NEXT_STATE.PC = CURRENT_STATE.PC + imm;
            }
            break;
        }



        default:
            printf("Instruction not implemented: 0x%X\n", opcode);
            break;
    }
}

