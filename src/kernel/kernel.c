#include <x16/kernel.h>

void kernel_init() {
    // 内存检测
    memory_check();
    BMB;
    // 开启 A20 地址线
    cli();                      // 关中断
    uint8_t v = inb(0x92);      // 开启 A20 地址线
    outb(0x92, v | 0x02);
    // 加载 GDT 
    init_gdt();
    BMB;
}