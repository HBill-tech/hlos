#include <kernel.h>

void test()
{
    char a = 'A';
}

void hlos_init(memory_info_t* mem_info, uint32_t gdt_info)
{
    test();
}