#include "disassembler.h"

const char* DEFAULT_ASSEMBLER_FILE_NAME    = "assembler.txt";
const char* DEFAULT_DISASSEMBLER_FILE_NAME = "disassembler.txt";

int main(int argc, char* argv[]) {
    const char* filename_disasm = DEFAULT_DISASSEMBLER_FILE_NAME;
    const char* filename_asm    = DEFAULT_ASSEMBLER_FILE_NAME;

    if (argc == 2) {
        filename_asm = argv[1];
    }
    else if (argc == 3) {
        filename_asm = argv[1];
        filename_disasm = argv[2];
    }

    CreateTextFromAssembler(filename_asm, filename_disasm);
}
           