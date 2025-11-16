// 函数的16位/32位特性由实现该函数的源文件决定，而非头文件决定
__asm__(".code16gcc");
#include <kernel.h>

void show_string(char *str) {
    // 遍历字符串，直到遇到空字符
    for (char *p = str; *p != 0; p++) {
        // *p & 0xFF 确保字符在 0 ~ 255, 字符编码占低 8 位。
        // 0x0E 占高 8 位，表示黄底黑字
        char c = (*p & 0xFF) | 0x0E00;

        // BIOS int 0x10  ah = 0x0E 表示电传打字输出功能
        __asm__ __volatile__(
            "mov $0xE, %%ah\n"      // AH = 0x0E 电传打字输出功能
            "mov %[c], %%al\n\t"    // AL = 要显示的字符     %[c] 会被替换为c变量的值
            "int $0x10\n"
            :
            : [c]"r"(c)             // [c]是操作数的别称, %[c]在编译的时候会被替换成一个通用寄存器名
        );
    }
}