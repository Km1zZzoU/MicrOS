#include "mode_y.h"
#include "drivers/vga/modes/registerfunctions.h"
#include "memory/heap/heap.h"
#include "drivers/dal/videocard/videocard.h"

//REGISTER VALUES
unsigned char g_320x200x256_modey[] =
	{
	/* MISC */
		0x63,
	/* SEQ */
		0x03, 0x01, 0x0F, 0x00, 0x06,
	/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28, 0x00, 0x96, 0xB9, 0xE3,
		0xFF,
	/* GC */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF,
	/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00, 0x00
	};

//PALETTE
unsigned char paletteY[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x2A, 0x00, 0x00, 0x2A, 0x2A, 0x2A, 0x00, 0x00, 0x2A,
    0x00, 0x2A, 0x2A, 0x15, 0x00, 0x2A, 0x2A, 0x2A, 0x15, 0x15, 0x15, 0x15, 0x15, 0x3F, 0x15, 0x3F,
    0x15, 0x15, 0x3F, 0x3F, 0x3F, 0x15, 0x15, 0x3F, 0x15, 0x3F, 0x3F, 0x3F, 0x15, 0x3F, 0x3F, 0x3F,
    0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x08, 0x08, 0x08, 0x0B, 0x0B, 0x0B, 0x0E, 0x0E, 0x0E, 0x11,
    0x11, 0x11, 0x14, 0x14, 0x14, 0x18, 0x18, 0x18, 0x1C, 0x1C, 0x1C, 0x20, 0x20, 0x20, 0x24, 0x24,
    0x24, 0x28, 0x28, 0x28, 0x2D, 0x2D, 0x2D, 0x32, 0x32, 0x32, 0x38, 0x38, 0x38, 0x3F, 0x3F, 0x3F,
    0x00, 0x00, 0x3F, 0x10, 0x00, 0x3F, 0x1F, 0x00, 0x3F, 0x2F, 0x00, 0x3F, 0x3F, 0x00, 0x3F, 0x3F,
    0x00, 0x2F, 0x3F, 0x00, 0x1F, 0x3F, 0x00, 0x10, 0x3F, 0x00, 0x00, 0x3F, 0x10, 0x00, 0x3F, 0x1F,
    0x00, 0x3F, 0x2F, 0x00, 0x3F, 0x3F, 0x00, 0x2F, 0x3F, 0x00, 0x1F, 0x3F, 0x00, 0x10, 0x3F, 0x00,
    0x00, 0x3F, 0x00, 0x00, 0x3F, 0x10, 0x00, 0x3F, 0x1F, 0x00, 0x3F, 0x2F, 0x00, 0x3F, 0x3F, 0x00,
    0x2F, 0x3F, 0x00, 0x1F, 0x3F, 0x00, 0x10, 0x3F, 0x1F, 0x1F, 0x3F, 0x27, 0x1F, 0x3F, 0x2F, 0x1F,
    0x3F, 0x37, 0x1F, 0x3F, 0x3F, 0x1F, 0x3F, 0x3F, 0x1F, 0x37, 0x3F, 0x1F, 0x2F, 0x3F, 0x1F, 0x27,
    0x3F, 0x1F, 0x1F, 0x3F, 0x27, 0x1F, 0x3F, 0x2F, 0x1F, 0x3F, 0x37, 0x1F, 0x3F, 0x3F, 0x1F, 0x37,
    0x3F, 0x1F, 0x2F, 0x3F, 0x1F, 0x27, 0x3F, 0x1F, 0x1F, 0x3F, 0x1F, 0x1F, 0x3F, 0x27, 0x1F, 0x3F,
    0x2F, 0x1F, 0x3F, 0x37, 0x1F, 0x3F, 0x3F, 0x1F, 0x37, 0x3F, 0x1F, 0x2F, 0x3F, 0x1F, 0x27, 0x3F,
    0x2D, 0x2D, 0x3F, 0x31, 0x2D, 0x3F, 0x36, 0x2D, 0x3F, 0x3A, 0x2D, 0x3F, 0x3F, 0x2D, 0x3F, 0x3F,
    0x2D, 0x3A, 0x3F, 0x2D, 0x36, 0x3F, 0x2D, 0x31, 0x3F, 0x2D, 0x2D, 0x3F, 0x31, 0x2D, 0x3F, 0x36,
    0x2D, 0x3F, 0x3A, 0x2D, 0x3F, 0x3F, 0x2D, 0x3A, 0x3F, 0x2D, 0x36, 0x3F, 0x2D, 0x31, 0x3F, 0x2D,
    0x2D, 0x3F, 0x2D, 0x2D, 0x3F, 0x31, 0x2D, 0x3F, 0x36, 0x2D, 0x3F, 0x3A, 0x2D, 0x3F, 0x3F, 0x2D,
    0x3A, 0x3F, 0x2D, 0x36, 0x3F, 0x2D, 0x31, 0x3F, 0x00, 0x00, 0x1C, 0x07, 0x00, 0x1C, 0x0E, 0x00,
    0x1C, 0x15, 0x00, 0x1C, 0x1C, 0x00, 0x1C, 0x1C, 0x00, 0x15, 0x1C, 0x00, 0x0E, 0x1C, 0x00, 0x07,
    0x1C, 0x00, 0x00, 0x1C, 0x07, 0x00, 0x1C, 0x0E, 0x00, 0x1C, 0x15, 0x00, 0x1C, 0x1C, 0x00, 0x15,
    0x1C, 0x00, 0x0E, 0x1C, 0x00, 0x07, 0x1C, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x1C, 0x07, 0x00, 0x1C,
    0x0E, 0x00, 0x1C, 0x15, 0x00, 0x1C, 0x1C, 0x00, 0x15, 0x1C, 0x00, 0x0E, 0x1C, 0x00, 0x07, 0x1C,
    0x0E, 0x0E, 0x1C, 0x11, 0x0E, 0x1C, 0x15, 0x0E, 0x1C, 0x18, 0x0E, 0x1C, 0x1C, 0x0E, 0x1C, 0x1C,
    0x0E, 0x18, 0x1C, 0x0E, 0x15, 0x1C, 0x0E, 0x11, 0x1C, 0x0E, 0x0E, 0x1C, 0x11, 0x0E, 0x1C, 0x15,
    0x0E, 0x1C, 0x18, 0x0E, 0x1C, 0x1C, 0x0E, 0x18, 0x1C, 0x0E, 0x15, 0x1C, 0x0E, 0x11, 0x1C, 0x0E,
    0x0E, 0x1C, 0x0E, 0x0E, 0x1C, 0x11, 0x0E, 0x1C, 0x15, 0x0E, 0x1C, 0x18, 0x0E, 0x1C, 0x1C, 0x0E,
    0x18, 0x1C, 0x0E, 0x15, 0x1C, 0x0E, 0x11, 0x1C, 0x14, 0x14, 0x1C, 0x16, 0x14, 0x1C, 0x18, 0x14,
    0x1C, 0x1A, 0x14, 0x1C, 0x1C, 0x14, 0x1C, 0x1C, 0x14, 0x1A, 0x1C, 0x14, 0x18, 0x1C, 0x14, 0x16,
    0x1C, 0x14, 0x14, 0x1C, 0x16, 0x14, 0x1C, 0x18, 0x14, 0x1C, 0x1A, 0x14, 0x1C, 0x1C, 0x14, 0x1A,
    0x1C, 0x14, 0x18, 0x1C, 0x14, 0x16, 0x1C, 0x14, 0x14, 0x1C, 0x14, 0x14, 0x1C, 0x16, 0x14, 0x1C,
    0x18, 0x14, 0x1C, 0x1A, 0x14, 0x1C, 0x1C, 0x14, 0x1A, 0x1C, 0x14, 0x18, 0x1C, 0x14, 0x16, 0x1C,
    0x00, 0x00, 0x10, 0x04, 0x00, 0x10, 0x08, 0x00, 0x10, 0x0C, 0x00, 0x10, 0x10, 0x00, 0x10, 0x10,
    0x00, 0x0C, 0x10, 0x00, 0x08, 0x10, 0x00, 0x04, 0x10, 0x00, 0x00, 0x10, 0x04, 0x00, 0x10, 0x08,
    0x00, 0x10, 0x0C, 0x00, 0x10, 0x10, 0x00, 0x0C, 0x10, 0x00, 0x08, 0x10, 0x00, 0x04, 0x10, 0x00,
    0x00, 0x10, 0x00, 0x00, 0x10, 0x04, 0x00, 0x10, 0x08, 0x00, 0x10, 0x0C, 0x00, 0x10, 0x10, 0x00,
    0x0C, 0x10, 0x00, 0x08, 0x10, 0x00, 0x04, 0x10, 0x08, 0x08, 0x10, 0x0A, 0x08, 0x10, 0x0C, 0x08,
    0x10, 0x0E, 0x08, 0x10, 0x10, 0x08, 0x10, 0x10, 0x08, 0x0E, 0x10, 0x08, 0x0C, 0x10, 0x08, 0x0A,
    0x10, 0x08, 0x08, 0x10, 0x0A, 0x08, 0x10, 0x0C, 0x08, 0x10, 0x0E, 0x08, 0x10, 0x10, 0x08, 0x0E,
    0x10, 0x08, 0x0C, 0x10, 0x08, 0x0A, 0x10, 0x08, 0x08, 0x10, 0x08, 0x08, 0x10, 0x0A, 0x08, 0x10,
    0x0C, 0x08, 0x10, 0x0E, 0x08, 0x10, 0x10, 0x08, 0x0E, 0x10, 0x08, 0x0C, 0x10, 0x08, 0x0A, 0x10,
    0x0B, 0x0B, 0x10, 0x0C, 0x0B, 0x10, 0x0D, 0x0B, 0x10, 0x0F, 0x0B, 0x10, 0x10, 0x0B, 0x10, 0x10,
    0x0B, 0x0F, 0x10, 0x0B, 0x0D, 0x10, 0x0B, 0x0C, 0x10, 0x0B, 0x0B, 0x10, 0x0C, 0x0B, 0x10, 0x0D,
    0x0B, 0x10, 0x0F, 0x0B, 0x10, 0x10, 0x0B, 0x0F, 0x10, 0x0B, 0x0D, 0x10, 0x0B, 0x0C, 0x10, 0x0B,
    0x0B, 0x10, 0x0B, 0x0B, 0x10, 0x0C, 0x0B, 0x10, 0x0D, 0x0B, 0x10, 0x0F, 0x0B, 0x10, 0x10, 0x0B,
    0x0F, 0x10, 0x0B, 0x0D, 0x10, 0x0B, 0x0C, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//DOUBLE BUFFER POINTER;

unsigned char *modey_buffer[] = {NULL, NULL, NULL, NULL};
unsigned char buffer_turned_on_y = 0;

int8_t modey_set_mode()
{
    writeRegisters(g_320x200x256_modey);
    set_vga_palette(paletteY);
    modey_clear_screen();
    video_card_set_turn_on_buffer_func(&modey_turn_on_buffer);
    video_card_set_turn_off_buffer_func(&modey_turn_off_buffer);
    video_card_set_is_buffer_on_func(&modey_is_buffer_on);
    video_card_set_swap_buffers_func(&modey_swap_buffers);
    video_card_set_draw_pixel_func(&modey_draw_pixel);
    video_card_set_draw_line_func(&modey_draw_line);
    video_card_set_draw_circle_func(&modey_draw_circle);
    video_card_set_draw_rectangle_func(&modey_draw_rectangle);
    video_card_set_clear_screen_func(&modey_clear_screen);

    video_card_set_print_char_func(NULL);
	video_card_set_print_char_color_func(NULL);
	video_card_set_print_string_func(NULL);
	video_card_set_print_string_color_func(NULL);
	video_card_set_set_char_func(NULL);
	video_card_set_get_char_func(NULL);
	video_card_set_set_color_func(NULL);
	video_card_set_get_color_func(NULL);
	video_card_set_set_char_and_color_func(NULL);
	video_card_set_get_char_and_color_func(NULL);
	video_card_set_set_cursor_pos_func(NULL);
	video_card_set_get_cursor_pos_func(NULL);
	video_card_set_turn_cursor_on_func(NULL);
	video_card_set_turn_cursor_off_func(NULL);
    return 0x69;
}

int8_t modey_turn_on_buffer()
{
    if(buffer_turned_on_y) return -1;
    for(int i = 3; i >= 0; i--)
    {
        modey_buffer[i] = heap_kernel_alloc(MODEY_HEIGHT * MODEY_WIDTH / 4, 0);
        if(modey_buffer[i] == NULL)
        {
            for(int j = 3; j >= i; j--)
            {
                heap_kernel_dealloc(modey_buffer[j]);
                modey_buffer[j] = NULL;
            }
            return -1;
        }
    }
    video_card_set_draw_pixel_func(&modey_draw_pixel_buffered);
    video_card_set_draw_line_func(&modey_draw_line_buffered);
    video_card_set_draw_circle_func(&modey_draw_circle_buffered);
    video_card_set_draw_rectangle_func(&modey_draw_rectangle_buffered);
    video_card_set_clear_screen_func(&modey_clear_screen_buffered);
    buffer_turned_on_y = 1;
    return 0;
}

int8_t modey_turn_off_buffer()
{
    if(!buffer_turned_on_y) return -1;
    for(int i = 3; i >= 0; i--)
    {
        heap_kernel_dealloc(modey_buffer[i]);
        modey_buffer[i] = NULL;
    }
    video_card_set_draw_pixel_func(&modey_draw_pixel);
    video_card_set_draw_line_func(&modey_draw_line);
    video_card_set_draw_circle_func(&modey_draw_circle);
    video_card_set_draw_rectangle_func(&modey_draw_rectangle);
    video_card_set_clear_screen_func(&modey_clear_screen);
    buffer_turned_on_y = 0;
    return 0;
}

uint8_t modey_is_buffer_on()
{
    return buffer_turned_on_y;
}

int8_t modey_swap_buffers()
{
    if(!buffer_turned_on_y) return -1;
    for(uint8_t p = 3; p < 4; p--)
    {
        set_plane(p);
        memcpy(VGA_VRAM, modey_buffer[p], MODEY_WIDTH * MODEY_HEIGHT / 4);
    }
    return 0;
}

int8_t modey_draw_pixel(uint8_t color, uint16_t x, uint16_t y)
{
    if((x>=MODEY_WIDTH) || (y >=MODEY_HEIGHT))
        return -1;
    unsigned char *fb = (unsigned char *) VGA_VRAM;
    unsigned int offset = (y * MODEY_WIDTH + x);
	set_plane(offset % 4);
    fb[offset/4] = color;
    set_plane(0);
    return 0;
}

int8_t modey_draw_line(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    if(ax == bx) return -1;
    int32_t dx = (int32_t)bx - ax;
    int32_t dy = (int32_t)by - ay;
    if(_abs(dx) >= _abs(dy))
    {
        float a = dy/(float)(dx);
        float b = ay - a * ax;
        if(ax > bx)
            for(int x = bx; x <= ax; ++x)
                modey_draw_pixel(color, x, a * x + b);
        else
            for(int x = ax; x <= bx; ++x)
                modey_draw_pixel(color, x, a * x + b);
    }
    else
    {
        float a = dx/(float)(dy);
        float b = ax - a * ay;
        if(ay > by)
            for(int y = by; y <= ay; ++ y)
                modey_draw_pixel(color, a * y + b, y);
        else
            for(int y = ay; y <= by; ++ y)
                modey_draw_pixel(color, a * y + b, y);
    }
    return 0;
}

int8_t modey_draw_circle(uint8_t color, uint16_t x, uint16_t y, uint16_t radius)
{
    return 0;
}

int8_t modey_draw_rectangle(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return 0;
}
int8_t modey_clear_screen()
{
    for(uint8_t p = 3; p < 4; p--)
    {
        set_plane(p);
        memset(VGA_VRAM, 0, 64 * 1024);
    }
    return 0;
}

int8_t modey_draw_pixel_buffered(uint8_t color, uint16_t x, uint16_t y)
{
    if((!buffer_turned_on_y) || (x>=MODEY_WIDTH) || (y >=MODEY_HEIGHT))
        return -1;
    unsigned int offset = (y * MODEY_WIDTH + x);
	modey_buffer[offset%4][offset/4] = color;
    return 0;
}

int8_t modey_draw_line_buffered(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    if(!buffer_turned_on_y) return -1;
    if(ax == bx) return -1;
    int32_t dx = (int32_t)bx - ax;
    int32_t dy = (int32_t)by - ay;
    if(_abs(dx) >= _abs(dy))
    {
        float a = dy/(float)(dx);
        float b = ay - a * ax;
        if(ax > bx)
            for(int x = bx; x <= ax; ++x)
                modey_draw_pixel_buffered(color, x, a * x + b);
        else
            for(int x = ax; x <= bx; ++x)
                modey_draw_pixel_buffered(color, x, a * x + b);
    }
    else
    {
        float a = dx/(float)(dy);
        float b = ax - a * ay;
        if(ay > by)
            for(int y = by; y <= ay; ++ y)
                modey_draw_pixel_buffered(color, a * y + b, y);
        else
            for(int y = ay; y <= by; ++ y)
                modey_draw_pixel_buffered(color, a * y + b, y);
    }
    return 0;
}
int8_t modey_draw_circle_buffered(uint8_t color, uint16_t x, uint16_t y, uint16_t radius)
{
    return 0;
}
int8_t modey_draw_rectangle_buffered(uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    return 0;
}
int8_t modey_clear_screen_buffered()
{
    if(!buffer_turned_on_y) return -1;
    for(uint8_t p = 3; p < 4; p--)
        memset(modey_buffer[p], 0, 64*1024);
    return 0;
}

int8_t modey_draw_pixel_external_buffer(uint8_t* buffer, uint16_t mode, int8_t color, uint16_t x, uint16_t y){
    if((x>=MODEY_WIDTH) || (y >=MODEY_HEIGHT))
        return -1;
    unsigned int offset = (y * MODEY_WIDTH + x);
	buffer[(offset%4) * MODEY_HEIGHT * MODEY_WIDTH / 4 + offset/4] = color;
    return 0;
}
int8_t modey_draw_line_external_buffer(uint8_t* buffer, uint16_t mode, uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by)
{
    if(ax == bx) return -1;
    int32_t dx = (int32_t)bx - ax;
    int32_t dy = (int32_t)by - ay;
    if(_abs(dx) >= _abs(dy))
    {
        float a = dy/(float)(dx);
        float b = ay - a * ax;
        if(ax > bx)
            for(int x = bx; x <= ax; ++x)
                modey_draw_pixel_external_buffer(buffer, mode, color, x, a * x + b);
        else
            for(int x = ax; x <= bx; ++x)
                modey_draw_pixel_external_buffer(buffer, mode, color, x, a * x + b);
    }
    else
    {
        float a = dx/(float)(dy);
        float b = ax - a * ay;
        if(ay > by)
            for(int y = by; y <= ay; ++ y)
                modey_draw_pixel_external_buffer(buffer, mode, color, a * y + b, y);
        else
            for(int y = ay; y <= by; ++ y)
                modey_draw_pixel_external_buffer(buffer, mode, color, a * y + b, y);
    }
    return 0;
}
int8_t modey_draw_circle_external_buffer(uint8_t* buffer, uint16_t mode, uint8_t color, uint16_t x, uint16_t y, uint16_t radius){
	return -1;
}
int8_t modey_draw_rectangle_external_buffer(uint8_t* buffer, uint16_t mode, uint8_t color, uint16_t ax, uint16_t ay, uint16_t bx, uint16_t by){
	return -1;
}
int8_t modey_clear_screen_external_buffer(uint8_t* buffer, uint16_t mode, uint16_t* x, uint16_t* y){
    memset(buffer, 0, 4*MODEY_HEIGHT * MODEY_WIDTH / 4);
    return 0;
}

int8_t modey_swap_external_buffer(uint8_t* buffer, uint16_t mode){
        for(uint8_t p = 3; p < 4; p--)
    {
        set_plane(p);
        memcpy(VGA_VRAM, buffer + p * MODEY_WIDTH * MODEY_HEIGHT / 4, MODEY_WIDTH * MODEY_HEIGHT / 4);
    }
    //memcpy(VGA_VRAM, mode13h_buffer, MODE13H_HEIGHT * MODE13H_WIDTH);
    return 0;
}
uint8_t* modey_create_external_buffer(uint16_t mode){
    uint8_t* ptr = heap_kernel_alloc(4*MODEY_HEIGHT * MODEY_WIDTH / 4, 0);
    memset(ptr, 0, 4*MODEY_HEIGHT * MODEY_WIDTH / 4);
    return ptr;
}