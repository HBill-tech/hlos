#ifndef HLOS_KERNEL_H
#define HLOS_KERNEL_H

#include <x16/os.h>
#include <types.h>

// 内核(x32)代码段和数据段的段选择子
#define KERNEL_CODE_SEG     (1 * 8)     // 二进制: 0000 0000 0000 1000
#define KERNEL_DATA_SEG     (2 * 8)     // 二进制: 0000 0000 0001 0000

/******************************** MEMORY CHECK ****************************/

// ARDS 描述符的最大数量
#define MEMORY_MAX_COUNT    24
// 一个 ARDS 结构体的字节数. 当 ARDS_SIZE = 24时, 可以兼容 20 位大小的 ARDS
#define ARDS_SIZE           24

// 一个 ARDS (内存区域描述符) 的结构
typedef struct memory_raw_t
{
    uint32_t base_l;
    uint32_t base_h;
    uint32_t length_l;
    uint32_t length_h;
    uint32_t type;
} memory_raw_t;

// 所有 ARDS 的数组
typedef struct  memory_info_t
{
    memory_raw_t raws[MEMORY_MAX_COUNT];
    uint32_t count;
} memory_info_t;



/******************************** GDT ****************************/
// 定义 GDT 表所容纳的最大记录数
#define GDT_SIZE            0x100       // 十进制: 256

// 以下是将 GDT 中 uint16_t attr 中每一个属性的可能取值定义为宏
#define SEG_ATTR_G          (1 << 15)
#define SEG_ATTR_D          (1 << 14)
#define SEG_ATTR_P          (1 << 7)

#define SEG_ATTR_DPL0       (0 << 5)
#define SEG_ATTR_DPL3       (3 << 5)

#define SEG_ATTR_CPL0       (0 << 0)
#define SEG_ATTR_CPL3       (3 << 0)

#define SEG_SYSTEM          (0 << 4)
#define SEG_NORMAL          (1 << 4)
#define SEG_TYPE_DATA       (0 << 3)
#define SEG_TYPE_CODE       (1 << 3)
#define SEG_TYPE_RW         (1 << 1)
#define SEG_TYPE_TSS        (9 << 0)


/**
 * gdt 格式
 *     | 31                    16bits                   16 | 15             16bits           0 |
 *     |                      Base 0:15                    |             Limit 0:15            |
 *     | 63     8bits   56 | 55 4bits 52 | 51    4bis   48 | 47    8bits    40 | 39  8bits  32 |
 *     |    Base 24:31     |    Flags    |   Limit 16:19   |    Access Byte    |   Base 16:23  |
 */
typedef struct gdt_table_t
{
    uint16_t limit_l;
    uint16_t base_l;
    uint8_t base_m;
    uint16_t attr;
    uint8_t base_h;
} _packed gdt_table_t;


/******************************** EXTERN FUNCTIONS ****************************/

// 输出字符串
void show_string(char *str);

// 内存检测
void memory_check();

// 初始化 gdt
void init_gdt();

// 进入保护模式
void protect_mode();

/******************************** INLINE OR STATIC FUNCTIONS ****************************/

// 从指定的16位端口号读取一个8位数据
static inline uint8_t  inb(uint16_t port) {
    uint8_t rv;
    __asm__ volatile(
        "inb %[p], %[v]"
        : [v]"=a"(rv)
        : [p]"d"(port)
    );
    return rv;
}

// 从指定的16位端口号读取一个16位 (word) 数据
static inline uint16_t inw(uint16_t port) {
    uint16_t rv;
    __asm__ volatile(
        "inw %1, %0"
        : "=a"(rv)
        : "dN"(port)
    );
    return rv;
}

// 向指定的16位端口写入一个8位(byte)数据
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile(
        "outb %[v], %[p]"
        :
        :[p]"d"(port), [v]"a"(data)
    );
}

// 向指定的16位端口写入一个16位(word)数据
static inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile(
        "outw %[v], %[p]"
        :
        :[p]"d"(port), [v]"a"(data)
    );
}

// 关中断
static inline void cli() {
    __asm__ volatile("cli");
}

// 开中断
static inline void sti() {
    __asm__ volatile("sti");
}

// 将GDT的基地址和界限加载到GDTR中
static inline void lgdt(uint32_t start, uint16_t size) {
    struct 
    {
        uint16_t limit;
        uint16_t start_l;
        uint16_t start_h;
    } gdt;
    
    gdt.start_h = start >> 16;
    gdt.start_l = start & 0xFFFF;
    gdt.limit = size - 1;
    __asm__ volatile(
        "lgdt %[g]"
        :
        :[g]"m"(gdt)
    );
}

// 读取 CR0 寄存器
static inline uint32_t read_cr0() {
    uint32_t cr0;
    __asm__ volatile(
        "mov %%cr0, %[v]"
        : [v]"=r"(cr0)
    );
    return cr0;
}

// 写入 cr0 寄存器
static inline void write_cr0(uint32_t v) {
    __asm__ volatile(
        "mov %[v], %%cr0"
        :
        : [v]"r"(v)
    );
}

// 远跳转
static inline void far_jump(uint32_t selector, uint32_t offset) {
    // 跳转地址的描述符
    // | 高 2 字节 | 低 4 字节 |
    // | selector  | offset   |
    uint32_t descriptor[] = { offset, selector };

    // 由于 x86 架构是小端序，因此都是从首地址(低字节)往后(高字节)截取界限长度作为有效数据
    // 内联汇编 ljmpl addr 从 addr (x86小端序，首地址存放低字节) 开始读取 6 个字节，
    // 前 4 个字节作为偏移量，后 2 个字节作为 selector
    __asm__ volatile(
        "ljmpl *(%[descriptor])"    // * : 从地址中读取内容
        :
        : [descriptor]"r"(descriptor)
    );
}

#endif