#ifndef LOGGER_H
#define LOGGER_H

#include "drivers/vga/vga.h"

void logger_log_info(char *content);
void logger_log_ok(char *content);
void logger_log_warning(char *content);
void logger_log_error(char *content);

void logger_replace_info(char *content);
void logger_replace_ok(char *content);
void logger_replace_warning(char *content);
void logger_replace_error(char *content);

void logger_log_critical(char *content);
void logger_print_on_screen(char *message, char *id, uint8_t id_color);
void logger_replace_last_log(char *message, char *id, uint8_t id_color);

#endif