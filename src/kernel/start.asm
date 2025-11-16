[bits 16]

extern kernel_init
global _start
_start:
    ; 清屏
    mov ah, 0x06
    mov bh, 0x07
    mov cx, 0
    mov dx, 0x184f
    int 0x10

    ; 设置视频模式
    mov ax, 0x3
    int 0x10

    jmp kernel_init

[bits 32]

KERNEL_CODE_SEG equ (1 * 8)
KERNEL_DATA_SEG equ (2 * 8)

extern kernel32_init
global protect_mode
protect_mode:
    ; 把除了 CS 之外的段寄存器全部设置为 KERNEL_DATA_SEG
    mov ax, KERNEL_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 远跳转自动将 CS 设为 KERNEL_CODE_SEG
    ; CS 不能通过 mov 指令赋值
    jmp KERNEL_CODE_SEG:kernel32_init