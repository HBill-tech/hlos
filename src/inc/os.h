#ifndef HLOS_OS_H
#define HLOS_OS_H

// 内核(x32)代码段和数据段的段选择子
#define KERNEL_CODE_SEG     (1 * 8)     // 二进制: 0000 0000 0000 1000
#define KERNEL_DATA_SEG     (2 * 8)     // 二进制: 0000 0000 0001 0000

#endif