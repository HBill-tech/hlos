[org 0x7c00]

; 读取内核代码
read_disk:
    ; bx=内存中的目标地址
    mov bx, 0x8000
    ; ch=磁道号 cl=扇区号
    mov cx, 0x01
    ; ah=2表示读扇区, 3表示写扇区 al=读取/写入的扇区数
    mov ax, 0x201
    ; dh=磁头号 dl=驱动器号(软驱从0开始, 硬盘从80H开始)
    mov dx, 0x80
    int 0x13
    ; 如果 CF = 1, 那么重新执行 read_disk. 如果 int13  ah = 02H 中断失败, CF = 1
    jc read_disk

    xchg bx, bx

times 510 - ($ - $$) db 0
dw 0xaa55