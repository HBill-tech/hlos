[org 0x7c00]

; edi表示内存中的目标位置
mov edi, 0x8000
; ecx 表示读取的起始扇区位置. 实际上 LBA28 模式只能用到其中的低 28 位
mov ecx, 0
; bl 表示读取的扇区数量
mov bl, 1

call read_disk

xchg bx, bx

read_disk:
    pushad  ; eax, ecx, edx, ebx, esp, ebp, esi, edi 依次压栈

    ; 设置扇区数量为 1
    mov dx, 0x1F2
    mov al, bl
    out dx, al  ; out 是给端口写入数据, in 是从端口读出数据

    ; 设置起始扇区为 0
    mov al, cl
    inc dx  ; dx  0x1F3
    out dx, al; 传入起始扇区第八位

    shr ecx, 8
    mov al, cl
    inc dx  ; 0x1F4
    out dx, al  ; 传入起始扇区的中八位

    shr ecx, 8
    mov al, cl
    inc dx  ;0x1F5
    out dx, al  ; 传入起始扇区的高八位

    shr ecx, 8
    ; cl 的低四位是 LBA 的最高四位
    and cl, 0b1111  ; 清零 cl 的高四位
    inc dx  ; 0x1F6
    mov al, 0b1110_0000
    or al, cl   ; al = 0x1110_[cl低四位]
    out dx, al

    inc dx  ; 0x1F7
    ; 设置为读硬盘模式
    mov al, 0x20
    out dx, al

    xor ecx, ecx
    mov cl, bl      ; 将要读取的扇区数量 bl 赋值给 cl

    .read:
        push cx     ; cx 压栈
        call .waits
        call .reads
        pop cx
        loop .read  ; 这里的 loop 循环次数也是根据 cx 决定的
        popad
        ret

    .waits:
        mov dx, 0x1F7
        ; 检测硬盘状态
        .check:
            nop
            nop
            nop
            ; 从端口读取数据
            in al, dx
            ; 检测是否数据准备完毕
            and al, 0b1000_1000
            cmp al, 0b0000_1000
            jnz .check
        ret
    .reads:
        mov dx, 0x1F0   ; 从该端口每次读入 2 Words / 16 bits
        mov cx, 0x100   ; 0x100 = 256, 总共读取 512 Bytes  .readw 每次读入 2 Bytes, 需要循环 265 次
        .readw:
            nop
            nop
            nop
            in ax, dx
            mov [edi], ax
            add edi, 2
            loop .readw     ; 默认 loop 每执行一次, cx 减 1 ，知道减为 0
        ret

times 510 - ($ - $$) db 0
dw 0xaa55