; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data
err_list:     db "Error: No se pudo crear la lista", 10, 0
err_node:     db "Error: No se pudo crear el nodo", 10, 0
err_result:   db "Error: No se pudo asignar memoria para el resultado", 10, 0

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat
extern strdup
extern fwrite
extern stderr

;-----------------------------------------------------------
; string_proc_list_create_asm:
;   Crea la lista. Ante error, escribe un mensaje y devuelve 0.
;-----------------------------------------------------------
string_proc_list_create_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 16

    mov     edi, 16         ; tamaño a asignar
    call    malloc
    mov     qword [rbp-8], rax

    cmp     qword [rbp-8], 0
    jne     .list_create_success

    ; Error en malloc: escribe mensaje de error.
    mov     rax, [rel stderr]
    mov     rcx, rax
    mov     edx, 33         ; longitud del mensaje (sin contar el terminador)
    mov     esi, 1
    mov     edi, err_list
    call    fwrite
    mov     eax, 0
    jmp     .exit

.list_create_success:
    mov     rax, [rbp-8]
    mov     qword [rax], 0
    mov     rax, [rbp-8]
    mov     qword [rax+8], 0
    mov     rax, [rbp-8]

.exit:
    leave
    ret

;-----------------------------------------------------------
; string_proc_node_create_asm:
;   Crea un nodo con un byte (parámetro en edi) y un puntero
;   (parámetro en rsi). Devuelve el puntero al nodo.
;-----------------------------------------------------------
string_proc_node_create_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32

    mov     eax, edi
    mov     qword [rbp-32], rsi
    mov     byte [rbp-20], al

    mov     edi, 32         ; tamaño del nodo
    call    malloc
    mov     qword [rbp-8], rax

    cmp     qword [rbp-8], 0
    jne     .node_create_success

    ; Error en malloc: escribe mensaje de error.
    mov     rax, [rel stderr]
    mov     rcx, rax
    mov     edx, 32
    mov     esi, 1
    mov     edi, err_node
    call    fwrite
    mov     eax, 0
    jmp     .exit

.node_create_success:
    mov     rax, [rbp-8]
    mov     qword [rax], 0
    mov     rax, [rbp-8]
    mov     qword [rax+8], 0
    mov     rax, [rbp-8]
    movzx   edx, byte [rbp-20]
    mov     byte [rax+16], dl
    mov     rax, [rbp-8]
    mov     rdx, [rbp-32]
    mov     qword [rax+24], rdx
    mov     rax, [rbp-8]

.exit:
    leave
    ret

;-----------------------------------------------------------
; string_proc_list_add_node_asm:
;   Agrega un nodo a la lista. Si la lista está vacía, el nodo
;   se agrega al comienzo. En caso de error, se escribe un mensaje.
;-----------------------------------------------------------
string_proc_list_add_node_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 48

    mov     qword [rbp-24], rdi    ; puntero a la lista
    mov     eax, esi             ; primer parámetro (byte)
    mov     qword [rbp-40], rdx    ; tercer parámetro (puntero)
    mov     byte [rbp-28], al
    movzx   eax, byte [rbp-28]
    mov     rdx, [rbp-40]
    mov     rsi, rdx
    mov     edi, eax
    call    string_proc_node_create_asm
    mov     qword [rbp-8], rax

    cmp     qword [rbp-8], 0
    jne     .add_valid

    ; Error al crear el nodo: escribe mensaje.
    mov     rax, [rel stderr]
    mov     rcx, rax
    mov     edx, 32
    mov     esi, 1
    mov     edi, err_node
    call    fwrite
    jmp     .exit

.add_valid:
    mov     rax, [rbp-24]
    mov     rax, [rax]          ; primer nodo de la lista
    test    rax, rax
    jne     .add_append
    mov     rax, [rbp-24]
    mov     rdx, [rbp-8]
    mov     qword [rax], rdx
    mov     rax, [rbp-24]
    mov     rdx, [rbp-8]
    mov     qword [rax+8], rdx
    jmp     .exit

.add_append:
    mov     rax, [rbp-24]
    mov     rax, [rax+8]
    mov     rdx, [rbp-8]
    mov     qword [rax], rdx
    mov     rax, [rbp-24]
    mov     rdx, [rax+8]
    mov     rax, [rbp-8]
    mov     qword [rax+8], rdx
    mov     rax, [rbp-24]
    mov     rdx, [rbp-8]
    mov     qword [rax+8], rdx

.exit:
    leave
    ret

;-----------------------------------------------------------
; string_proc_list_concat_asm:
;   Concatena las cadenas de los nodos de la lista que cumplan
;   una condición (criterio en esi). Se duplica la cadena hash 
;   (puntero en rdx). En caso de error, se escribe un mensaje.
;-----------------------------------------------------------
string_proc_list_concat_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 64

    mov     qword [rbp-40], rdi     ; puntero al primer nodo de la lista
    mov     eax, esi              ; criterio (byte)
    mov     qword [rbp-56], rdx     ; puntero a la cadena hash
    mov     byte [rbp-44], al

    cmp     qword [rbp-40], 0
    je      .concat_null
    cmp     qword [rbp-56], 0
    jne     .concat_duplicate

.concat_null:
    mov     eax, 0
    jmp     .exit

.concat_duplicate:
    mov     rax, [rbp-56]
    mov     rdi, rax
    call    strdup
    mov     qword [rbp-8], rax
    cmp     qword [rbp-8], 0
    jne     .concat_dup_ok

    mov     rax, [rel stderr]
    mov     rcx, rax
    mov     edx, 52
    mov     esi, 1
    mov     edi, err_result
    call    fwrite
    mov     eax, 0
    jmp     .exit

.concat_dup_ok:
    mov     rax, [rbp-40]
    mov     rax, [rax]
    mov     qword [rbp-16], rax
    jmp     .concat_loop

.concat_process:
    mov     rax, [rbp-16]
    movzx   eax, byte [rax+16]
    cmp     byte [rbp-44], al
    jne     .concat_next
    mov     rax, [rbp-16]
    mov     rdx, [rax+24]
    mov     rax, [rbp-8]
    mov     rsi, rdx
    mov     rdi, rax
    call    str_concat
    mov     qword [rbp-24], rax
    mov     rax, [rbp-8]
    mov     rdi, rax
    call    free
    mov     rax, [rbp-24]
    mov     qword [rbp-8], rax

.concat_next:
    mov     rax, [rbp-16]
    mov     rax, [rax]
    mov     qword [rbp-16], rax

.concat_loop:
    cmp     qword [rbp-16], 0
    jne     .concat_process
    mov     rax, [rbp-8]

.exit:
    leave
    ret
