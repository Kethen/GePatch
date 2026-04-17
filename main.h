#ifndef __MAIN_H
#define __MAIN_H

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspge.h>
#include <pspdmac.h>
#include <systemctrl.h>

#define ENABLE_LOGGING 1

#if ENABLE_LOGGING
#define log(...) \
{ \
    char _msg[256]; \
    int _len = sprintf(_msg,__VA_ARGS__); \
    logmsg(_msg, _len); \
}

#define LOG_FILE "ms0:/ge_patch.txt"

void logmsg(char *msg, int len);
#else
#define log(...)
#endif

typedef struct {
    u32 list;
    u32 offset;
} StackEntry;

typedef struct {
    u32 ge_cmds[0x100];

    u32 texbufptr[8];
    u32 texbufwidth[8];
    u32 framebufptr;
    u32 framebufwidth;
    u32 *framebufwidth_addr;

    u32 base;
    u32 offset;
    u32 address;

    u32 index_addr;
    u32 vertex_addr;
    u32 vertex_type;

    u32 ignore_framebuf;
    u32 ignore_texture;

    u32 sync;
    u32 finished;

    StackEntry stack[64];
    u32 curr_stack;

    u32 framebuf_addr[16];
    u32 framebuf_count;
} GeState;

extern GeState state;

void getVertexInfo(u32 op, u8 *vertex_size, u8 *pos_off, u8 *visit_off, int *pos_size);

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#endif
