#include "mode_07h.h"
#include "drivers/vga/modes/registerFunctions.h"
#include "memory/heap/heap.h"
#include "drivers/dal/videocard/videocard.h"
#include "drivers/vga/vga.h"

//REGISTER VALUES
uint8_t g_80x25_texth[] =
	{
        /* MISC */
        0x66, 
        /* SEQ */
        0x03, 0x00, 0x03, 0x00, 0x02, 
        /* CRTC */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 
        /* GC */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0A, 0x00,
        0xFF, 
        /* AC */
        0x00, 0x01, 0x08, 0x03, 0x08, 0x05, 0x08, 0x07,
        0x10, 0x09, 0x18, 0x0B, 0x18, 0x0D, 0x18, 0x0F,
        0x0E, 0x01, 0x0F, 0x13, 0x00
		};

//DOUBLE BUFFER POINTER;
screen *mode07h_buffer = NULL;

int8_t mode07h_set_mode()
{
    return -1;
}

int8_t mode07h_turn_on_buffer()
{
    return -1;
}
int8_t mode07h_turn_off_buffer()
{
    return -1;
}
uint8_t mode07h_is_buffer_on()
{
    return -1;
}

int8_t mode07h_swap_buffers()
{
    return -1;
}

int8_t mode07h_draw_pixel(uint8_t color, uint16_t x, uint16_t y)
{
    return -1;
}
int8_t mode07h_draw_line(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode07h_draw_circle(uint8_t color, uint16_t x, uint16_t y, uint16_t radius)
{
    return -1;
}
int8_t mode07h_draw_rectangle(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode07h_clear_screen()
{
    return -1;
}

int8_t mode07h_draw_pixel_buffered(uint8_t color, uint16_t x, uint16_t y)
{
    return -1;
}
int8_t mode07h_draw_line_buffered(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode07h_draw_circle_buffered(uint8_t color, uint16_t x, uint16_t y, uint16_t radius)
{
    return -1;
}
int8_t mode07h_draw_rectangle_buffered(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode07h_clear_screen_buffered()
{
    return -1;
}