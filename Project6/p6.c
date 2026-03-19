#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <hex_logical_address>\n", argv[0]);
        return 1;
    }

    char *endptr = NULL;
    unsigned long parsed = strtoul(argv[1], &endptr, 16);

    if (*argv[1] == '\0' || *endptr != '\0')
    {
        fprintf(stderr, "Invalid hexadecimal address: %s\n", argv[1]);
        return 1;
    }

    uint32_t logical_addr = (uint32_t)parsed;
    uint32_t page_index = logical_addr >> 12;
    uint32_t offset = logical_addr & 0xFFF;

    printf("Logical Addr:0x%08X - Page Index:0x%08X - Offset:0x%08X\n",
           logical_addr,
           page_index,
           offset);

    return 0;
}
