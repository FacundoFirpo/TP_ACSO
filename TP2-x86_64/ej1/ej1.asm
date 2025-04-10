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

    mov rdx, rsi      ; guardamos hash en rdx porque lo vamos a necesitar luego
    mov rsi, rdi      ; guardamos type en rsi

    mov rdi, 32       ; tamaño del nodo: 4 punteros (8*4 = 32 bytes)
    call malloc
    test rax, rax
    je .malloc_fail

    ; rax = puntero al nodo
    ; Inicializar campos
    mov qword [rax], NULL         ; next
    mov qword [rax + 8], NULL     ; previous
    mov byte  [rax + 16], sil     ; type (rsi -> sil)
    mov qword [rax + 24], rdx     ; hash

    ; devolver nodo en rax
    pop rbp
    ret

.malloc_fail:
    mov rax, NULL
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
    push rbx                ; vamos a usar rbx para current_node
    push r12                ; result
    push r13                ; temp

    ; Validaciones
    test rdi, rdi
    je .return_null
    test rdx, rdx
    je .return_null

    ; strdup(hash)
    mov rdi, rdx
    extern strdup
    call strdup
    test rax, rax
    je .return_null
    mov r12, rax            ; r12 = result

    ; recorrer lista
    mov rbx, [rdi]          ; list->first

.loop:
    test rbx, rbx
    je .done

    mov al, [rbx + 16]      ; node->type (byte)
    cmp al, sil             ; comparar con type
    jne .next

    ; concatenar result + node->hash
    mov rdi, r12            ; result
    mov rsi, [rbx + 24]     ; node->hash
    call str_concat
    ; liberar viejo result
    mov rdi, r12
    call free
    mov r12, rax            ; nuevo result

.next:
    mov rbx, [rbx]          ; current_node = current_node->next
    jmp .loop

.done:
    mov rax, r12            ; devolver result
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

.return_null:
    mov rax, NULL
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret