#include "mode_02h.h"
#include "drivers/vga/modes/registerFunctions.h"
#include "memory/heap/heap.h"
#include "drivers/dal/videocard/videocard.h"
#include "drivers/vga/vga.h"

//REGISTER VALUES
uint8_t g_80x25_textm[] =
	{
        /* MISC */
        0x67, 
        /* SEQ */
        0x03, 0x00, 0x03, 0x00, 0x02, 
        /* CRTC */
        0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
        0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
        0xFF, 
        /* GC */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
        0xFF, 
        /* AC */
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x0C, 0x00, 0x0F, 0x08, 0x00
		};

//DOUBLE BUFFER POINTER;
screen *mode02h_buffer = NULL;

int8_t mode02h_set_mode()
{
    return -1;
}

int8_t mode02h_turn_on_buffer()
{
    return -1;
}
int8_t mode02h_turn_off_buffer()
{
    return -1;
}
uint8_t mode02h_is_buffer_on()
{
    return -1;
}

int8_t mode02h_swap_buffers()
{
    return -1;
}

int8_t mode02h_draw_pixel(uint8_t color, uint16_t x, uint16_t y)
{
    return -1;
}
int8_t mode02h_draw_line(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode02h_draw_circle(uint8_t color, uint16_t x, uint16_t y, uint16_t radius)
{
    return -1;
}
int8_t mode02h_draw_rectangle(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode02h_clear_screen()
{
    return -1;
}

int8_t mode02h_draw_pixel_buffered(uint8_t color, uint16_t x, uint16_t y)
{
    return -1;
}
int8_t mode02h_draw_line_buffered(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode02h_draw_circle_buffered(uint8_t color, uint16_t x, uint16_t y, uint16_t radius)
{
    return -1;
}
int8_t mode02h_draw_rectangle_buffered(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return -1;
}
int8_t mode02h_clear_screen_buffered()
{
    return -1;
}