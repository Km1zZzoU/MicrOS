#include "micros_partitions.h"

int micros_partitions_get_count()
{
    return micros_interrupt_0a(0xB0);
}

void micros_partitions_get_symbols(char *symbols_array)
{
    micros_interrupt_1a(0xB1, symbols_array);
}