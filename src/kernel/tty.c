__asm__(".code16gcc");
#include <x16/kernel.h>

void show_string(char *str, uint8_t color) {
    // 这里的 color 实际上没有被用到
    // 若 color > 255 , 则 color = color - n * 256，使得 color 在 0 ~ 255 间 
    color = color & 0xFF;       // 确保颜色值在0-255范围内

    // 遍历字符串，直到遇到空字符
    for (char *p = str; *p != 0; p++) {
        // *p & 0xFF 确保字符在 0 ~ 255, 字符编码占低 8 位。
        // 0x0E 占高 8 位，表示黄底黑字
        char c = (*p & 0xFF) | 0x0E00;

        // BIOS int 0x10  ah = 0x0E 表示电传打字输出功能
        __asm__ volatile(
            "mov $0xE, %%ah\n"      // AH = 0x0E 电传打字输出功能
            "mov %[c], %%al\n\t"    // AL = 要显示的字符     %[c] 会被替换为c变量的值
            "int $0x10\n"
            :
            : [c]"r"(c), "b"(color) // [c]是操作数的别称, %[c]在编译的时候会被替换成一个通用寄存器名
        );
    }
}