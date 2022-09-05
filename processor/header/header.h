#ifndef HEADER_H__
#define HEADER_H__

const int SIGNATURE_ASM     = 48613;
const int VER_ASSEMBLER_CMD = 1;

struct HEADER_ASM_FILE {
    int signature = SIGNATURE_ASM;
    int ver_asm = VER_ASSEMBLER_CMD;
    size_t size_in_byte = 0;
};

#define HEADER_INIT(header, size)            \
    header.signature    = SIGNATURE_ASM;     \
    header.ver_asm      = VER_ASSEMBLER_CMD; \
    header.size_in_byte = size;

#endif