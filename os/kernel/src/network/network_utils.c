#include "network_utils.h"

uint8_t __uint8_flip(uint8_t value)
{
    uint8_t out = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        out = out << 1;

        out |= ((value >> i) & 0x01);
    }

    return out;
}

uint16_t __uint16_flip(uint16_t value)
{
    uint8_t bytes[2] = {
        (uint8_t)(*((uint8_t *)&value)),
        (uint8_t)(*((uint8_t *)&value + 1))};
    return (bytes[0] << 8) | (bytes[1]);
}

uint32_t __uint32_flip(uint32_t value)
{
    uint8_t bytes[4] = {
        (uint8_t)(*((uint8_t *)&value)),
        (uint8_t)(*((uint8_t *)&value + 1)),
        (uint8_t)(*((uint8_t *)&value + 2)),
        (uint8_t)(*((uint8_t *)&value + 3)),
    };
    return (bytes[0]) << 24 | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3]);
}

bool __compare_mac_address(uint8_t *first, uint8_t *second)
{
    if (first == 0 || second == 0)
        return false;

    for (char i = 0; i < 6; i++)
    {
        if (*(first + i) != *(second + i))
            return false;
    }

    return true;
}

bool __compare_ip_address(uint8_t *first, uint8_t *second)
{
    if (first == 0 || second == 0)
        return false;

    for (char i = 0; i < 4; i++)
    {
        if (*(first + i) != *(second + i))
            return false;
    }

    return true;
}