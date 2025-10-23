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