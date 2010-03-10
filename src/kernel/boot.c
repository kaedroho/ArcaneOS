#include "boot.h"

struct multiboot_information* g_multiboot_info = 0;

struct multiboot_information* get_multiboot_info()
{
    return g_multiboot_info;
}
