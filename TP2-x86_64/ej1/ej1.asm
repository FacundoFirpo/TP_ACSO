; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat


string_proc_list_create_asm:
    ; reservar espacio para string_proc_list (dos punteros)
    push rbp
    mov rbp, rsp

    mov rdi, 16               ; 2 punteros de 8 bytes = 16 bytes
    call malloc
    test rax, rax
    je .malloc_fail

    ; rax contiene el puntero a la estructura
    mov qword [rax], NULL     ; first = NULL
    mov qword [rax + 8], NULL ; last = NULL

    mov rax, rax              ; devolver puntero (ya está en rax)
    pop rbp
    ret

.malloc_fail:
    ; imprimir mensaje de error (puede omitirse o implementar con extern fprintf si querés)
    mov rax, NULL
    pop rbp
    ret

; argumentos:
; rdi = type (uint8_t)
; rsi = hash (char*)

string_proc_node_create_asm:
    push rbp
    mov rbp, rsp
    push rbx

    ; Save type in a safe register
    mov rbx, rdi

    ; Call strdup to create a copy of the hash string
    mov rdi, rsi
    extern strdup
    call strdup
    test rax, rax
    je .malloc_fail
    
    ; Save the duplicated string
    mov rsi, rax

    ; Now allocate memory for the node
    mov rdi, 32       ; tamaño del nodo: 4 punteros (8*4 = 32 bytes)
    call malloc
    test rax, rax
    je .strdup_fail

    ; rax = puntero al nodo
    ; Inicializar campos
    mov qword [rax], NULL         ; next
    mov qword [rax + 8], NULL     ; previous
    mov byte  [rax + 16], bl      ; type (rbx -> bl)
    mov qword [rax + 24], rsi     ; hash (duplicated string)

    ; devolver nodo en rax
    pop rbx
    pop rbp
    ret

.strdup_fail:
    ; Free the duplicated string if node allocation fails
    mov rdi, rsi
    call free
    mov rax, NULL
    pop rbx
    pop rbp
    ret

.malloc_fail:
    mov rax, NULL
    pop rbx
    pop rbp
    ret

; argumentos:
; rdi = puntero a string_proc_list (list)
; rsi = type
; rdx = hash

string_proc_list_add_node_asm:
    push rbp
    mov rbp, rsp

    ; llamar a string_proc_node_create_asm(type, hash)
    mov r8, rdi        ; guardar list en r8
    mov rdi, rsi       ; type
    mov rsi, rdx       ; hash
    call string_proc_node_create_asm
    test rax, rax
    je .end            ; si no se pudo crear nodo, salir

    ; rax = nuevo nodo
    mov r9, rax        ; guardar nodo en r9

    mov rax, r8               ; rax = list
    mov rcx, [rax]            ; list->first
    test rcx, rcx
    je .list_empty

    ; lista no está vacía
    mov rdx, [rax + 8]        ; list->last
    mov [rdx], r9             ; last->next = node
    mov [r9 + 8], rdx         ; node->previous = last
    mov [rax + 8], r9         ; list->last = node
    jmp .end

.list_empty:
    ; lista vacía: list->first = node; list->last = node
    mov [rax], r9             ; list->first = node
    mov [rax + 8], r9         ; list->last = node

.end:
    pop rbp
    ret

; argumentos:
; rdi = list
; rsi = type
; rdx = hash

string_proc_list_concat_asm:
    push rbp
    mov rbp, rsp
    sub rsp, 16               ; Reserve stack space for local variables
    push rbx                  ; vamos a usar rbx para current_node
    push r12                  ; result
    push r13                  ; list pointer
    push r14                  ; type
    push r15                  ; hash pointer

    ; Validaciones
    test rdi, rdi
    jz .return_null
    test rdx, rdx
    jz .return_null

    ; Save parameters in non-volatile registers
    mov r13, rdi              ; Save list pointer in r13
    mov r14, rsi              ; Save type in r14
    mov r15, rdx              ; Save hash pointer in r15

    ; strdup(hash)
    mov rdi, r15
    call strdup
    test rax, rax
    jz .return_null
    mov r12, rax              ; r12 = result (duplicated hash)

    ; recorrer lista
    mov rbx, [r13]            ; list->first

.loop:
    test rbx, rbx
    jz .done

    ; Check if node type matches requested type
    movzx eax, byte [rbx + 16]  ; node->type (zero-extend to avoid garbage)
    cmp al, r14b                ; compare with type
    jne .next

    ; Get node hash
    mov rsi, [rbx + 24]       ; node->hash
    test rsi, rsi             ; check if hash is NULL
    jz .next                  ; skip if NULL

    ; Concatenate result + node->hash
    mov rdi, r12              ; result
    call str_concat
    test rax, rax             ; Check if str_concat returned NULL
    jz .next                  ; Skip if NULL
    
    ; Free old result and update with new concatenated string
    mov rdi, r12
    mov r12, rax              ; Save new result before freeing old one
    call free

.next:
    mov rbx, [rbx]            ; current_node = current_node->next
    jmp .loop

.done:
    mov rax, r12              ; Return result
    jmp .cleanup

.return_null:
    mov rax, NULL

.cleanup:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    add rsp, 16
    pop rbp
    ret