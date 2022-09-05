#include "cpu.h"
#include "../base_lib/base_lib.h"
#include "../header/header.h"
#include "../commands/commands.h"
#include "../logger/logger.h"

#define InitAsmCode(cpu, filename_assembler) InitAsmCode__(cpu, filename_assembler, LOCATION_STRUCT_CALL__{LOCATION__})
size_t InitAsmCode__(CPU* cpu, const char* filename_assembler, LOCATION_STRUCT_CALL__ call_place);

int CPUConstructor(CPU* cpu);
int CPUDestructor (CPU* cpu);

const int MAKE_NULL_FLAGS_BYTE = 31;

static const char* NAME_MODULE_CPU = "CPU";
                                                    
/*----------------------------------------*/

#define DEF_CMD(cmd_in, num_args, is_leftside_arg, ident_of_type, command_action)                         \
    else if (((int)cpu.code[cpu.ip] & MAKE_NULL_FLAGS_BYTE) == (int)PROCESSOR_COMMANDS::CMD_##cmd_in) {   \
        command_action                                                                                    \
    }

int ExecuteCPU(const char* filename_assembler) {
    CPU cpu = {};
    CPUConstructor(&cpu);

    size_t size_code_byte = InitAsmCode(&cpu, filename_assembler);

    for (; cpu.ip < size_code_byte;) {
        if (1 == 0) {

        }

        #include "../commands/cmd_def.h"

        else {
            CreateLog(NAME_MODULE_CPU, "Failed: Cant recognize command", LOCATION_STRUCT_CALL__(), LOCATION_VAR__(cpu));
            CPUDestructor(&cpu);
            return -1;
        }
    }

    return 0;
}

#undef DEF_CMD

int CPUConstructor(CPU* cpu) {
    StackConstructor((cpu->stack));
    StackConstructor((cpu->stack_call));
    cpu->code = nullptr;
    cpu->ip = 0;
    cpu->reg = {};
    cpu->ram_memory.ram = (CPU_ARG_INT_T*)calloc(cpu->ram_memory.size_ram + cpu->ram_memory.size_video_ram, sizeof(CPU_ARG_INT_T));
    if (_IsBadReadPtr(cpu->ram_memory.ram)) {
        CreateLog(NAME_MODULE_CPU, "Failed: Cant get memory for RAM", LOCATION_STRUCT_CALL__(), LOCATION_VAR__(cpu->ram_memory.ram));
    }

    return 0;
}

int CPUDestructor(CPU* cpu) {
    StackDestructor(&(cpu->stack));
    StackDestructor(&(cpu->stack_call));
    free(cpu->ram_memory.ram);
    free(cpu->code);

    return 0;
}

size_t InitAsmCode__(CPU* cpu, const char* filename_assembler, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(cpu)) {
        CreateLog(NAME_MODULE_CPU, "Failed: Bad ptr", call_place, LOCATION_VAR__(cpu));
        return -1;
    }

    FILE* file_assembler = fopen(filename_assembler, "rb");
    if (_IsBadReadPtr(file_assembler)) {
        CreateLog(NAME_MODULE_CPU, "Failed: Cant open file to read assembler code", call_place, LOCATION_VAR__(file_assembler));
    }
    
    HEADER_ASM_FILE header = {};
    fread(&header, sizeof(HEADER_ASM_FILE), 1, file_assembler);
    fseek(file_assembler, sizeof(HEADER_ASM_FILE), SEEK_SET);

    if (header.ver_asm != VER_ASSEMBLER_CMD) {
        CreateLog(NAME_MODULE_CPU, "Version of header assembler file do not equal current version. Please re-create assembler file.", call_place, LOCATION_VAR__(header));
        return -1;
    }

    cpu->code = (char*)calloc(header.size_in_byte, sizeof(char));
    if (_IsBadReadPtr(cpu->code)) {
        CreateLog(NAME_MODULE_CPU, "Failed: Cant calloc memory", call_place, LOCATION_VAR__(cpu->code));
        return -1;
    }

    fread(cpu->code, sizeof(char), header.size_in_byte, file_assembler);

    fclose(file_assembler);

    return header.size_in_byte;
}