.section .text
.global br_test

br_test:
    // Guardar el valor de los registros que usaremos
    stp x29, x30, [sp, -16]!  
    mov x29, sp

    // Configurar valores de prueba
    ldr x0, =target_address  // Cargar la direccion de destino en x0
    br x0                    // Saltar a la direccion contenida en x0

fail:
    mov x1, #0xDEAD          // Indicar fallo si llega aqui
    b end

target_address:
    .text
    // Test BR (Branch to Register) instruction
    MOVZ X1, #10     // Initialize X1 with 10

    // Store the address of label1 in X10
    ADR X10, label1

    // Branch to the address in X10
    BR X10

    // This code should be skipped
    MOVZ X2, #20     // This should be skipped
    MOVZ X3, #30     // This should be skipped

    label1:
    // This code should be executed after the branch
    MOVZ X4, #40     // X4 = 40
    MOVZ X5, #50     // X5 = 50

    // Halt
    HLT 0

end:
    // Restaurar registros y retornar
    ldp x29, x30, [sp], 16
    ret
