__asm__(".code16gcc");
#ifndef HLOS_X_16_KERNEL_H
#define HLOS_X_16_KERNEL_H

#include <types.h>

#define BMB __asm__ volatile("xchgw %bx, %bx")

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

// 定义一个全局可以访问的所有 ARDS 数组
extern memory_info_t memory_info;

// 内存检测
void memory_check();

// 输出字符串
void show_string(char *str, uint8_t color);

#endif