#include "vbe.h"
#include "../v8086/v8086.h"
#include "../v8086/memory_operations.h"
#include <stdbool.h>
#include <string.h>
#include "../memory/heap/heap.h"

v8086* machine = NULL;
bool initialized = false;

void VBE_initialize()
{
    if(!initialized)
    {
        machine = v8086_create_machine();
        if(machine == NULL) return;
        v8086_set_386_instruction_set(machine);
        initialized = true;
    }
}

void VBE_close()
{
    if(initialized)
    {
        v8086_destroy_machine(machine);
        initialized = false;
    }
}

bool VBE_is_initialized()
{
    return initialized;
}

VBEStatus VBE_check_existance_of_VESA()
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f03;
    v8086_call_int(machine, 0x10);
    if(machine->regs.x.ax != 0x004f) return VBE_NOT_EXIST;
    return VBE_OK;
}

VBEStatus VBE_get_svga_information(svga_information** information_struct_ptr){
    if(!initialized) return VBE_NO_INITAILIZED;
    svga_information* information_struct = heap_kernel_alloc(sizeof(svga_information), 0);
    if(information_struct == NULL) return VBE_INTERNAL_ERROR;
    machine->regs.x.ax = 0x4f00;
    machine->sregs.es = 0x0000;
    machine->regs.x.di = 0x7E00;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *(uint32_t*)(information_struct->signature) = read_dword_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E00));
    information_struct->signature[5] = '\0';
    information_struct->vesa_standard_number = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E04));
    uint16_t off = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E06));
    uint16_t seg = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E08));
    uint16_t len = strlen(machine->Memory + get_absolute_address(seg, off));
    information_struct->producent_text = heap_kernel_alloc(len + 1, 0);
    strcpy(information_struct->producent_text, machine->Memory + get_absolute_address(seg, off));
    information_struct->additional_info = read_dword_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E0A));
    off = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E0E));
    seg = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E10));
    for(len = 0; read_word_from_pointer(machine->Memory, get_absolute_address(seg, off + 2*len)) != 0xffff; ++len);
    information_struct->number_of_modes = len;
    information_struct->mode_array = heap_kernel_alloc(2 * len, 0);
    memcpy(information_struct->mode_array, machine->Memory + get_absolute_address(seg, off), len * 2);
    *information_struct_ptr = information_struct;
    return VBE_OK;
}

VBEStatus VBE_get_vesa_mode_information(svga_mode_information* infromation_struct, uint16_t mode_number)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f01;
    machine->regs.x.cx = mode_number;
    machine->sregs.es = 0x0000;
    machine->regs.x.di = 0x7E00;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    infromation_struct->mode_attributes = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E00));
    infromation_struct->window_a_attributes = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E02));
    infromation_struct->window_b_attributes = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E03));
    infromation_struct->granularity = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E04));
    infromation_struct->windows_size = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E06));
    infromation_struct->segment_of_window_a = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E08));
    infromation_struct->segment_of_window_b = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E0A));
    infromation_struct->far_pointer_to_bank_swap_function = read_dword_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E0C));
    infromation_struct->logical_line_length = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E10));
    infromation_struct->mode_width = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E12));
    infromation_struct->mode_height = read_word_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E14));
    infromation_struct->char_matrix_width = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E16));
    infromation_struct->char_matrix_height = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E17));
    infromation_struct->plane_count = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E18));
    infromation_struct->bits_per_pixel = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E19));
    infromation_struct->bank_count = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E1A));
    infromation_struct->memory_organization = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E1B));
    infromation_struct->bank_size = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E1C));
    infromation_struct->page_count = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E1D));
    infromation_struct->reserved = read_byte_from_pointer(machine->Memory, get_absolute_address(0x0000, 0x7E1E));

    return VBE_OK;
}

VBEStatus VBE_destroy_svga_information(svga_information* svga_information_ptr)
{
    heap_kernel_dealloc(svga_information_ptr->mode_array);
    heap_kernel_dealloc(svga_information_ptr->producent_text);
    heap_kernel_dealloc(svga_information_ptr);
    return VBE_OK;
}

VBEStatus VBE_set_video_mode(uint16_t mode_number, bool clear_screen)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f02;
    machine->regs.x.bx = mode_number & 0x7FFF;
    if(clear_screen) machine->regs.x.bx | 0x8000;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    return VBE_OK;
}

VBEStatus VBE_get_current_video_mode(uint16_t* mode_number)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f03;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *mode_number = machine->regs.x.bx;
    return VBE_OK;
}

VBEStatus VBE_return_save_restore_state_buffer_size(uint16_t requested_states, uint16_t* buffer_block_number)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f04;
    machine->regs.h.dl = 0x00;
    machine->regs.x.cx = requested_states;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *buffer_block_number = machine->regs.x.bx;
    return VBE_OK;
}

VBEStatus VBE_save_restore_state(bool save, uint16_t requested_states, uint16_t buffer_pointer)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f04;
    machine->regs.h.dl = save? 0x01 : 0x02;
    machine->regs.x.cx = requested_states;
    machine->sregs.es = buffer_pointer;
    machine->regs.x.bx = 0x00;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    return VBE_OK;
}

VBEStatus VBE_display_window_control_set_16bit(uint8_t window_number, uint8_t window_mem_number)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f05;
    machine->regs.h.bh = 0x00;
    machine->regs.h.bl = window_number;
    machine->regs.x.dx = window_mem_number;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    return VBE_OK;
}

VBEStatus VBE_display_window_control_get_16bit(uint8_t window_number, uint8_t* window_mem_number)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f05;
    machine->regs.h.bh = 0x01;
    machine->regs.h.bl = window_number;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *window_mem_number = machine->regs.x.dx;
    return VBE_OK;
}

VBEStatus VBE_display_window_control_set_32bit(uint8_t window_number, uint8_t window_mem_number, uint16_t memory_selector)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.h.bh = 0x00;
    machine->regs.h.bl = window_number;
    machine->regs.x.dx = window_mem_number;
    machine->sregs.es = memory_selector;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    return VBE_OK;
}

VBEStatus VBE_get_logical_scan_line_length(bool get_maximum_length, uint16_t* bytes_per_line, uint16_t* actual_pixel_in_line, uint16_t* maximum_scan_lines_number)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f06;
    machine->regs.h.bl = get_maximum_length? 0x03: 0x01;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *bytes_per_line = machine->regs.x.bx;
    *actual_pixel_in_line = machine->regs.x.cx;
    *maximum_scan_lines_number = machine->regs.x.dx;
    return VBE_OK;
}

VBEStatus VBE_set_logical_scan_line_length(bool in_pixels, uint16_t length, uint16_t* bytes_per_line, uint16_t* actual_pixel_in_line, uint16_t* maximum_scan_lines_number)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f06;
    machine->regs.h.bl = in_pixels? 0x00: 0x02;
    machine->regs.x.cx = length;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *bytes_per_line = machine->regs.x.bx;
    *actual_pixel_in_line = machine->regs.x.cx;
    *maximum_scan_lines_number = machine->regs.x.dx;
    return VBE_OK;
}

//set/get display start

VBEStatus VBE_set_dac_palette_format(uint8_t primary_color_bits, uint8_t* curent_number_color_bits)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f08;
    machine->regs.h.bl = 0x00;
    machine->regs.h.bh = primary_color_bits;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *curent_number_color_bits = machine->regs.h.bh;
    return VBE_OK;
}

VBEStatus VBE_get_dac_palette_format(uint8_t* curent_number_color_bits)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f08;
    machine->regs.h.bl = 0x01;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *curent_number_color_bits = machine->regs.h.bh;
    return VBE_OK;
}

VBEStatus VBE_get_set_palette_data_16bit(uint8_t action, uint8_t palette_registers_num, uint16_t first_palette_register, uint16_t palette_table)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f09;
    machine->regs.h.bl = action;
    machine->regs.x.cx = palette_registers_num;
    machine->regs.x.dx = first_palette_register;
    machine->sregs.es = palette_table;
    machine->regs.x.di = 0x00;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    return VBE_OK;
}

VBEStatus VBE_get_set_palette_data_32bit(bool during_vertical_retrace, uint8_t palette_registers_num, uint16_t first_palette_register, uint16_t palette_table, uint16_t memory_selector)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f09;
    machine->regs.h.bl = during_vertical_retrace? 0x80 : 0x00;
    machine->regs.x.cx = palette_registers_num;
    machine->regs.x.dx = first_palette_register;
    machine->sregs.es = palette_table;
    machine->regs.d.edi = 0x00;
    machine->sregs.ds = memory_selector;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    return VBE_OK;
}

VBEStatus VBE_get_protected_mode_interface(uint16_t* real_mode_table_segment, uint16_t* table_offset, uint16_t* table_length)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f0a;
    machine->regs.h.bl = 0x01;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *real_mode_table_segment = machine->sregs.es;
    *table_offset = machine->regs.x.di;
    *table_length = machine->regs.x.cx;
    return VBE_OK;
}

VBEStatus VBE_get_set_pixel_clock(uint16_t pixel_clock, uint16_t mode_number, uint16_t* closest_pixel_clock)
{
    if(!initialized) return VBE_NO_INITAILIZED;
    machine->regs.x.ax = 0x4f0b;
    machine->regs.h.bl = 0x00;
    machine->regs.d.ecx = pixel_clock;
    machine->regs.x.dx = mode_number;
    int16_t status = v8086_call_int(machine, 0x10);
    if(status != 0x10) return VBE_INTERNAL_ERROR;
    if(machine->regs.h.al != 0x4f) return VBE_NOT_EXIST;
    if(machine->regs.h.ah != 0x00) return VBE_FUNCTION_FAILURE;
    *closest_pixel_clock = machine->regs.d.ecx;
    return VBE_OK;
}