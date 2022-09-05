#include "disassembler.h"
#include "../base_lib/base_lib.h"
#include "../header/header.h"
#include "../commands/commands.h"
#include "../constants/constants.h"
#include "../types/types.h"
#include "../logger/logger.h"

#define WriteCommand(command, buffer, count_byte, num_args, ident_of_type, file_disassembler) WriteCommand__(command, buffer, count_byte, num_args,  \
                                                                                                             ident_of_type, file_disassembler,       \
                                                                                                             LOCATION_STRUCT_CALL__{LOCATION__})
int WriteCommand__(const char* command, char* buffer, size_t* count_byte, int num_args, int ident_of_type, FILE* file_disassembler, LOCATION_STRUCT_CALL__ call_place);

#define WriteArgument(buffer, count_byte, ident_of_type, file_disassembler) WriteArgument__(buffer, count_byte, ident_of_type,     \
                                                                                            file_disassembler,                     \
                                                                                            LOCATION_STRUCT_CALL__{ LOCATION__ })
int WriteArgument__(char* buffer, size_t* count_byte, int ident_of_type, FILE* file_disassembler, LOCATION_STRUCT_CALL__ call_place);

static int LogSyntaxiceDisassembler(const char* err_syntax);

const int MAKE_NULL_FLAGS_BYTE = 31;      

static const char* LOG_SYNTAX_FILE_NAME = "log_syntax.txt";
static const char* NAME_MODULE_DISASM   = "DISASSEMBLER";

/*----------------------------------------*/

#define DEF_CMD(cmd_name, num_args, is_leftside_arg, ident_of_type, ...)                                        \
    else if (((int)buffer[count_byte] & MAKE_NULL_FLAGS_BYTE) == (int)PROCESSOR_COMMANDS::CMD_##cmd_name) {     \
        WriteCommand(#cmd_name, buffer, &count_byte, num_args, ident_of_type, file_disassembler);               \
    }

int CreateTextFromAssembler(const char* filename_assembler, const char* filename_output) {
    FILE* file_assembler = fopen(filename_assembler, "rb");
    if (_IsBadReadPtr(file_assembler)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Cant open file with assembler", LOCATION_STRUCT_CALL__(), LOCATION_VAR__(file_assembler));
        return -1;
    }

    HEADER_ASM_FILE header = {};
    fread(&header, sizeof(HEADER_ASM_FILE), 1, file_assembler);
    fseek(file_assembler, sizeof(HEADER_ASM_FILE), SEEK_SET);

    if (header.ver_asm != VER_ASSEMBLER_CMD) {
        CreateLog(NAME_MODULE_DISASM, "Version of header assembler file do not equal current version. Please re-create assembler file.", LOCATION_STRUCT_CALL__(),
                        LOCATION_VAR__(header.ver_asm));
        return -1;
    }

    char* buffer = (char*)calloc(header.size_in_byte, sizeof(char));
    size_t num_read_symb = fread(buffer, sizeof(char), header.size_in_byte, file_assembler);

    fclose(file_assembler);

    FILE* file_disassembler = fopen(filename_output, "w");
    if (_IsBadReadPtr(file_assembler)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Cant open file to write disassembler code", LOCATION_STRUCT_CALL__(), LOCATION_VAR__(file_assembler));
        return -1;
    }

    for (size_t count_byte = 0; count_byte < num_read_symb;) {
        if (1 == 0) {

        }

        #include "../commands/cmd_def.h"

        else {
            LogSyntaxiceDisassembler("Failed: Disassembler cant recognize symb");
            return -1;
        }
    }

    fclose(file_disassembler);

    return 0;
}

#undef DEF_CMD

int WriteCommand__(const char* command, char* buffer, size_t* count_byte, int num_args, int ident_of_type, FILE* file_disassembler, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(buffer)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Bad ptr", call_place, LOCATION_VAR__(buffer));
        return -1;
    }
    if (_IsBadReadPtr(count_byte)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Bad ptr", call_place, LOCATION_VAR__(count_byte));
        return -1;
    }
    if (_IsBadReadPtr(file_disassembler)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Cant open file to write disassembler code", call_place, LOCATION_VAR__(file_disassembler));
        return -1;
    }

    fprintf(file_disassembler, "%s", command);
    if (num_args > 0) {
        WriteArgument(buffer, count_byte, ident_of_type, file_disassembler);
    }
    else {
        ++(*count_byte);
    }
    fprintf(file_disassembler, "\n");

    return 0;
}

int WriteArgument__(char* buffer, size_t* count_byte, int ident_of_type, FILE* file_disassembler, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(buffer)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Bad ptr", call_place, LOCATION_VAR__(buffer));
        return -1;
    }
    if (_IsBadReadPtr(count_byte)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Bad ptr", call_place, LOCATION_VAR__(count_byte));
        return -1;
    }
    if (_IsBadReadPtr(file_disassembler)) {
        CreateLog(NAME_MODULE_DISASM, "Failed: Cant open file to write disassembler code", call_place, LOCATION_VAR__(file_disassembler));
        return -1;
    }
    if (ident_of_type == 0) {
        bool is_mem = (int)buffer[*count_byte] & RAM_ARG_CMD;
        bool is_reg = (int)buffer[*count_byte] & REG_ARG_CMD;
        bool is_const = (int)buffer[*count_byte] & CONST_ARG_CMD;

        ++(*count_byte);

        char           reg[2] = { 0 };
        CPU_ARG_REAL_T arg_const = 0;

        if (is_reg) reg[0] = buffer[(*count_byte)++];
        if (is_const) {
            arg_const = (CPU_ARG_REAL_T)((CPU_ARG_REAL_T)*((CPU_ARG_INT_T*)(buffer + *count_byte)) / (CPU_ARG_REAL_T)PRECISION);
            (*count_byte) += sizeof(CPU_ARG_INT_T);
        }

        if (is_mem) {
            fprintf(file_disassembler, " [");
            if (is_reg && is_const) {
                fprintf(file_disassembler, "%cx+%lld", reg[0], (CPU_ARG_INT_T)arg_const);
            }
            else if (is_reg) {
                fprintf(file_disassembler, "%cx", reg[0]);
            }
            else if (is_const) {
                fprintf(file_disassembler, "%lld", (CPU_ARG_INT_T)arg_const);
            }
            fprintf(file_disassembler, "]");
        }
        else {
            if (is_reg && is_const) {
                fprintf(file_disassembler, " %cx+%.3f", reg[0], arg_const);
            }
            else if (is_reg) {
                fprintf(file_disassembler, " %cx", reg[0]);
            }
            else if (is_const) {
                fprintf(file_disassembler, " %.3f", arg_const);
            }
        }
    }
    else if (ident_of_type == 1) {
        ++(*count_byte);
        fprintf(file_disassembler, " %c", buffer[(*count_byte)]);
        ++(*count_byte);
        for (; buffer[(*count_byte)] != '$'; ++(*count_byte)) {
            fprintf(file_disassembler, "%c", buffer[(*count_byte)]);
        }
        fprintf(file_disassembler, "%c", buffer[(*count_byte)]);
        ++(*count_byte);
    }

    return 0;
}

static int LogSyntaxiceDisassembler(const char* err_syntax) {
    fprintf(stderr, "Disassembler detect syntaxis error: %s.\n", err_syntax);

    FILE* file_log = fopen(LOG_SYNTAX_FILE_NAME, "a");
    if (_IsBadReadPtr(file_log)) {
        printf("Disasembler has error: Cant open log file\n");
        return -1;
    }

    setvbuf(file_log, (char*)NULL, _IONBF, 0);
    fprintf(file_log, "Disassembler detect syntaxis error: %s.\n", err_syntax);

    fclose(file_log);

    return 0;
}
