#ifndef CPU_H__
#define CPU_H__

/*--------------------------------------------------------------------------*/

#include <cstddef>
#include <math.h>
/*--------------------------------------------------------------------------*/

#include "../types/types.h"
#include "../constants/constants.h"
#include "graphics/graphics.h"
#include "stack_lib/stack_lib.h"

/*--------------------------------------------------------------------------*/


#ifdef __linux__
    #define LINUX_CODE(...) __VA_ARGS__
#elif _WIN32
    #define LINUX_CODE(...)
#endif

/*--------------------------------------------------------------------------*/

struct REGISTER {
    CPU_ARG_INT_T registr[4] = {};
};

const int RAM_BYTE       = 1024;
const int RAM_VIDEO_BYTE = HEIGHT_WINDOW * WIDTH_WINDOW;

struct RAM {
    size_t size_ram       = RAM_BYTE;
    size_t size_video_ram = RAM_VIDEO_BYTE;
    CPU_ARG_INT_T* ram = nullptr;
};

struct CPU {
    REGISTER reg = {};
    RAM      ram_memory = {};
    size_t   ip = 0;
    char* code = {};
    Stack_t stack = {};
    Stack_t stack_call = {};
};

/*--------------------------------------------------------------------------*/

int    ExecuteCPU  (const char* filename_assembler);
size_t InitAsmCode (CPU* cpu, const char* filename_assembler);

#endif
