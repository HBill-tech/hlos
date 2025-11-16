__asm__(".code16gcc");
#include <kernel.h>

memory_info_t memory_info;

void memory_check() {
    int sign32 = 0x534D4150, sign16 = 0xE820;
    int index = 0, signature, bytes;    
    while (TRUE)
    {
        memory_raw_t *memory_raw = &memory_info.raws[index];
        /**
         * input:   
         *      memory_raw: ARDS缓冲区。中断会将内存段信息写回到输入的 memory_raw
         *      sign32 & sign16: 执行内存检测的"验证码"
         *      ARDS_SIZE: 一个 ARDS 结构体的字节数. 当 ARDS_SIZE = 24时, 可以兼容 20 位大小的 ARDS
         *      index: 上一轮内存检测的 index，如果本轮是第一轮那么 index = 0
         * output:
         *      bytes: 表示在 ES:DI 所指向的缓冲区(memory_raw)中实际写入的字节数
         *      index: 中断给出的下一个内存段的索引，如果为 0 表示内存检测完成
         *      signature: 中断返回的签名，用来判断内存检测是否成功
         */
        __asm__ __volatile__(
            "int $0x15"
            : "=a"(signature), "=c"(bytes), "=b"(index)          
            : "a"(sign16), "b"(index), "c"(ARDS_SIZE), "d"(sign32), "D"(memory_raw) 
        );
        // 如果内存检测签名不是 sign32，则检测失败
        if (signature != sign32)
        {
            show_string("Memory check error!\r\n");
        }
        // 记录已经检测的内存段数
        memory_info.count ++;
        // index 再次为 0, 表示内存检测完毕
        if (index == 0)
        {
            show_string("Memory check success\r\n");
            break;
        }
    }
}