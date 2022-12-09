#include "debug.h"

void handle_stack_overflow() {
    zemu_log("----------- CANARY TRIGGERED -----------\n");
    zemu_log("-------- STACK OVERFLOW DETECTED ---------- \n");

#if TARGET_NAME == TARGET_NANOS || TARGET_NAME == TARGET_NANOX
    io_seproxyhal_se_reset();
#else
    while (1);
#endif
}

void check_app_canary() {
#if TARGET_NAME == TARGET_NANOS || TARGET_NAME == TARGET_NANOX
    if (app_stack_canary != APP_STACK_CANARY_MAGIC) handle_stack_overflow();
#endif
}


void zemu_log_stack(char *ctx) {
#if defined(ZEMU_LOGGING)
#if TARGET_NAME == TARGET_NANOS || TARGET_NAME == TARGET_NANOX
#define STACK_SHIFT 20
    void* p = NULL;
    char buf[70];
    snprintf(buf, sizeof(buf), "|SP| %p %p (%d) : %s\n",
             &app_stack_canary,
             ((void*)&p)+STACK_SHIFT,
             (uint32_t)((void*)&p)+STACK_SHIFT - (uint32_t)&app_stack_canary,
             ctx);
    zemu_log(buf);
#endif
#endif
}

void zemu_log(char *buf)
{
#if defined(ZEMU_LOGGING)
#if TARGET_NAME == TARGET_NANOS || TARGET_NAME == TARGET_NANOX
    asm volatile (
        "movs r0, #0x04\n"
        "movs r1, %0\n"
        "svc      0xab\n"
        :: "r"(buf) : "r0", "r1"
    );
#endif
#endif
}