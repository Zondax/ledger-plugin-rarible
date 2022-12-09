#pragma once
#include <stdint.h>

#define APP_STACK_CANARY_MAGIC 0xDEAD0031
#define NULL ((void *)0)
extern unsigned int app_stack_canary;

// Zemu and canary functions
void check_app_canary();
void zemu_log_stack(char *ctx);
void zemu_log(char *buf);

// Printf that uses speculos semi-hosting features.
void semihosted_printf(const char *format, ...);