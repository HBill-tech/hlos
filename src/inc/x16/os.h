__asm__(".code16gcc");
#ifndef HLOS_X16_OS_H
#define HLOS_X16_OS_H

#define BMB __asm__ volatile("xchgw %bx, %bx")

#endif