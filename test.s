.intel_syntax noprefix
.globl main
main:
    push rbp
    mov rbp, rsp
.BB0:
    mov DWORD PTR -8[rbp], 5
    mov eax, DWORD PTR -8[rbp]
    mov DWORD PTR -4[rbp], eax
    jmp .BB1
.BB1:
    jmp .BB3
.BB3:
    mov DWORD PTR -16[rbp], 1
    mov eax, DWORD PTR -4[rbp]
    sub eax, DWORD PTR -16[rbp]
    mov DWORD PTR -20[rbp], eax
    mov eax, DWORD PTR -20[rbp]
    mov DWORD PTR -4[rbp], eax
    jmp .BB4
.BB4:
    cmp DWORD PTR -4[rbp], 0
    je .BB2
    jmp .BB1
.BB2:
    mov eax, DWORD PTR -4[rbp]
    mov DWORD PTR -12[rbp], eax
    jmp .main_ret
.main_ret:
    mov eax, DWORD PTR -12[rbp]
    pop rbp
    ret
