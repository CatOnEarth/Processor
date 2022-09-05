#include "assembler.h"

const char* DEFAULT_FILENAME_CODE       = "code.txt";
const char* DEFAULT_ASSEMBLER_FILE_NAME = "assembler.txt"; 

int main(int argc, char* argv[]) {
    const char* filename_code = DEFAULT_FILENAME_CODE;
    const char* filename_asm  = DEFAULT_ASSEMBLER_FILE_NAME;

    if (argc == 2) {
        filename_code = argv[1];
    }
    else if (argc == 3) {
        filename_code = argv[1];
        filename_asm = argv[2];
    }

    CreateAssemblerFile(filename_code, filename_asm);
}
