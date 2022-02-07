#include "micros_power.h"

void micros_power_reboot()
{
    micros_interrupt_0a(0x0C00);
}

void micros_power_shutdown()
{
    micros_interrupt_0a(0x0C01);
}