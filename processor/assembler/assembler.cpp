#include "assembler.h"
#include "assembler_constants.h"
#include "../commands/commands.h"
#include "../types/types.h"
#include "../constants/constants.h"
#include "../header/header.h"
#include "../logger/logger.h"

/*----------------------------------------*/

struct LABEL_ {
    char name_label[MAX_LABEL_LENGTH] = { 0 };
    int  pos = 0;
};

/*----------------------------------------*/

#define InputTextCode(programm_code_text, file_code_text) InputTextCode__(programm_code_text, file_code_text, LOCATION_STRUCT_CALL__{LOCATION__})
int     InputTextCode__(TextStruct* programm_code_text, FILE* file_code_text, LOCATION_STRUCT_CALL__ call_place);

#define TextSeparate(text) TextSeparate__(text, LOCATION_STRUCT_CALL__{LOCATION__})
int     TextSeparate__(TextStruct* text, LOCATION_STRUCT_CALL__ call_place);

#define ChangeWordsToCodes(programm_code_text, filename_assembler_text, count_labels, labels_arr, step)       \
        ChangeWordsToCodes__(programm_code_text, filename_assembler_text, count_labels, labels_arr,  step,    \
                             LOCATION_STRUCT_CALL__{LOCATION__})
int     ChangeWordsToCodes__(TextStruct* programm_code_text, const char* filename_assembler_text, size_t* count_labels, LABEL_* labels_arr, int step,
                             LOCATION_STRUCT_CALL__ call_place);

#define SeparateTextByEndOfLine(text) SeparateTextByEndOfLine__(text, LOCATION_STRUCT_CALL__{LOCATION__})
int     SeparateTextByEndOfLine__(TextStruct* text, LOCATION_STRUCT_CALL__ call_place);

#define ParseCommand(num_cmd, commands_arr, ip, is_was_hlt) ParseCommand__(num_cmd, commands_arr, ip, is_was_hlt, LOCATION_STRUCT_CALL__{LOCATION__})
char*   ParseCommand__(int num_cmd, char* commands_arr, size_t* ip, bool* is_was_hlt, LOCATION_STRUCT_CALL__ call_place);

#define ParseArgument(commands_arr, cmd, cur_line, ip, pointer_to_command, args, is_leftside_arg, count_labels, labels_arr, step)       \
        ParseArgument__(commands_arr, cmd, cur_line, ip, pointer_to_command, args, is_leftside_arg, count_labels, labels_arr, step, LOCATION_STRUCT_CALL__{LOCATION__})
int     ParseArgument__(char* commands_arr, char* cmd, int num_line, size_t* ip, char* pointer_to_command, char* args, bool is_leftside_arg,
                        size_t* count_labels, LABEL_* labels_arr, int step, LOCATION_STRUCT_CALL__ call_place);

bool IsCorrectRegister(char reg);

#define FindPosLabel(label, count_labels, labels_arr) FindPosLabel__(label, count_labels, labels_arr, LOCATION_STRUCT_CALL__{LOCATION__}) 
int     FindPosLabel__(const char* label, size_t count_labels, LABEL_* labels_arr, LOCATION_STRUCT_CALL__ call_place);

#define SetPosLabel(label, cur_line, ip, count_labels, labels_arr) SetPosLabel__(label, cur_line, ip, count_labels, labels_arr,  LOCATION_STRUCT_CALL__{LOCATION__})
int     SetPosLabel__(char* label, int cur_line, size_t ip, size_t* count_labels, LABEL_* labels_arr, LOCATION_STRUCT_CALL__ call_place);

#define WriteAssemblerToFile(code_arr, size, filename_assembler_text)                                        \
        WriteAssemblerToFile__(code_arr, size, filename_assembler_text,  LOCATION_STRUCT_CALL__{LOCATION__})
int     WriteAssemblerToFile__(char* code_arr, size_t size, const char* filename_assembler_text, LOCATION_STRUCT_CALL__ call_place);

static int LogSyntaxiceAssembler(const char* err_syntax, char* asm_str, int asm_line);

static const char* NAME_MODULE_ASM = "ASSEMBLER";

static const char* LOG_SYNTAX_FILE_NAME = "log_syntax.txt";

/*----------------------------------------*/

int CreateAssemblerFile__(const char* filename_code, const char* filename_asm, LOCATION_STRUCT_CALL__ call_place) {
    if (filename_code == nullptr) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(filename_code));
        return -1;
    }
    if (filename_asm == nullptr) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(filename_asm));
        return -1;
    }

    FILE* file_input = fopen(filename_code, "r");
    if (_IsBadReadPtr(file_input)) {
        CreateLog(NAME_MODULE_ASM, "Cant open text file with code", LOCATION_STRUCT_CALL__{LOCATION__}, LOCATION_VAR__(file_input));
        return -1;
    }

    TextStruct code_programm = {};
    TextConstructor(&code_programm);
    GetMemoryForTextBuf(&code_programm, file_input);

    InputTextCode(&code_programm, file_input);

    size_t count_labels = 0;
    LABEL_ labels_arr[MAX_LABELS_COUNT] = { 0 };

    ChangeWordsToCodes(&code_programm, filename_asm, &count_labels, labels_arr, 1);
    ChangeWordsToCodes(&code_programm, filename_asm, &count_labels, labels_arr, 2);

    fclose(file_input);

    TextDestructor(&code_programm);

    return 0;
}

int InputTextCode__(TextStruct* code_programm, FILE* file_input, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(code_programm)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(code_programm));
        return -1;
    }
    if (_IsBadReadPtr(file_input)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(file_input));
        return -1;
    }

    fread(code_programm->text.str, sizeof(char), getFileSize(file_input) + 1, file_input);
    code_programm->text.lenght = strlen(code_programm->text.str);

    TextSeparate(code_programm);

    return 0;
}

int TextSeparate__(TextStruct* text, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(text)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(text));
        return -1;
    }

    text->num_strings = CountNumOfStrings(&text->text);

    text->strings_text = (StringStruct*)calloc(text->num_strings + 1, sizeof(StringStruct));
    if (_IsBadReadPtr(text->strings_text)) {
        CreateLog(NAME_MODULE_ASM, "Cant calloc memory", LOCATION_STRUCT_CALL__(), LOCATION_VAR__(text->strings_text));
        return -1;
    }

    SeparateTextByEndOfLine(text);

    return 0;
}

int SeparateTextByEndOfLine__(TextStruct* text, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(text)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(text));
        return -1;
    }

    size_t cur_size_str = text->text.lenght;
    size_t count_symb_in_cur_line = 0;
    size_t n_line = 0;
    for (size_t num_symb = 0; num_symb < cur_size_str + 1; ++num_symb) {
        if (text->text.str[num_symb] == '\n' || text->text.str[num_symb] == '\r' || text->text.str[num_symb] == '\0') {
            text->text.str[num_symb] = '\0';

            text->strings_text[n_line].str = (text->text.str + num_symb - count_symb_in_cur_line);

            if (strlen(text->strings_text[n_line].str) == 0) {
                count_symb_in_cur_line = 0;
                --text->num_strings;
                continue;
            }

            text->strings_text[n_line].lenght = strlen(text->strings_text[n_line].str);

            count_symb_in_cur_line = 0;
            ++n_line;
        }
        else if (text->text.str[num_symb] == ';') {
            while (text->text.str[num_symb] != '\n' && text->text.str[num_symb] != '\r' && text->text.str[num_symb] != '\0') {
                text->text.str[num_symb] = '\0';
                ++count_symb_in_cur_line;
            }
        }
        else {
            ++count_symb_in_cur_line;
        }
    }

    return 0;
}

#define DEF_CMD(cmd_name, num_args, is_leftside_arg, ...)                                                                      \
    else if (strcmp(cmd, #cmd_name) == 0) {                                                                                    \
        pointer_to_command = ParseCommand((int)(PROCESSOR_COMMANDS::CMD_##cmd_name), commands_arr, &ip, &is_was_hlt);          \
        if (num_args > 0) {                                                                                                    \
            ParseArgument(commands_arr, cmd, cur_line, &ip, pointer_to_command, args, is_leftside_arg, count_labels, labels_arr, step);  \
        }                                                                                                                      \
    }  

int ChangeWordsToCodes__(TextStruct* programm_code_text, const char* filename_assembler_text, size_t* count_labels, LABEL_* labels_arr, int step,
                         LOCATION_STRUCT_CALL__ call_place) {

    if (_IsBadReadPtr(programm_code_text)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(programm_code_text));
        return -1;
    }
    if (_IsBadReadPtr(count_labels)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(count_labels));
        return -1;
    }
    if (_IsBadReadPtr(labels_arr)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(labels_arr));
        return -1;
    }

    char    commands_arr[MAX_CODE_LENGTH] = { 0 };
    char    cmd[MAX_COMMAND_LENGTH] = { 0 };
    char    args[MAX_COMMAND_LENGTH] = { 0 };
    int     symb_to_end_cmd = -1;
    size_t  ip = 0;
    char* pointer_to_command = commands_arr;
    bool    is_was_hlt = false;

    int num_strings_in_code = programm_code_text->num_strings;
    for (int cur_line = 0; cur_line < num_strings_in_code; ++cur_line) {
        sscanf(programm_code_text->strings_text[cur_line].str, "%s%n", cmd, &symb_to_end_cmd);
        sscanf(programm_code_text->strings_text[cur_line].str + symb_to_end_cmd, " %[^\n]s", args);

        if (1 == 0) {

        }

        #include "../commands/cmd_def.h"

        else if (cmd[strlen(cmd) - 1] == ':' && strlen(cmd) >= 2) {
            if (step == 1) {
                cmd[strlen(cmd) - 1] = '\0';
                SetPosLabel(cmd, cur_line, ip, count_labels, labels_arr);
            }
        }
        else {
            LogSyntaxiceAssembler("Failed: Cant recognize symbol", cmd, cur_line);
        }
    }

    if (!is_was_hlt) {
        LogSyntaxiceAssembler("Failed: Cant find end programm(HLT)", cmd, -1);
    }

    if (step == 2) {
        WriteAssemblerToFile(commands_arr, ip, filename_assembler_text);
    }

    return 0;
}

char* ParseCommand__(int num_cmd, char* commands_arr, size_t* ip, bool* is_was_hlt, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(commands_arr)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(commands_arr));
        return nullptr;
    }
    if (_IsBadReadPtr(ip)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(ip));
        return nullptr;
    }
    if (_IsBadReadPtr(is_was_hlt)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(is_was_hlt));
        return nullptr;
    }

    char* ip_command = commands_arr + (*ip);

    commands_arr[(*ip)] = (char)(num_cmd);
    if (num_cmd == (int)PROCESSOR_COMMANDS::CMD_HLT) {
        (*is_was_hlt) = true;
    }

    ++(*ip);

    return ip_command;
}

int ParseArgument__(char* commands_arr, char* cmd, int num_line, size_t* ip, char* pointer_to_command, char* args, bool is_leftside_arg,
                    size_t* count_labels, LABEL_* labels_arr, int step, LOCATION_STRUCT_CALL__ call_place) {

    if (_IsBadReadPtr(commands_arr)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(commands_arr));
        return -1;
    }
    if (_IsBadReadPtr(ip)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(ip));
        return -1;
    }
    if (_IsBadReadPtr(pointer_to_command)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(pointer_to_command));
        return -1;
    }
    if (_IsBadReadPtr(args)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(args));
        return -1;
    }
    if (_IsBadReadPtr(count_labels)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(count_labels));
        return -1;
    }
    if (_IsBadReadPtr(labels_arr)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(labels_arr));
        return -1;
    }

    bool  is_const = false;
    bool  is_reg = false;
    bool  is_mem = false;

    char arg_label[MAX_LABEL_LENGTH] = { 0 };
    char str_arg[MAX_STRING_LENGTH]  = { 0 };
    char reg = 0;
    CPU_ARG_INT_T  pos = 0;
    int  count_symb_to_end = -1;

    CPU_ARG_INT_T  tmp_const = 0;
    CPU_ARG_REAL_T arg_const = 0;

    if ((sscanf(args, "[%cx+%lf]%n", &reg, &arg_const, &count_symb_to_end) == 2 ||
        sscanf(args, "[%lf+%cx]%n", &arg_const, &reg, &count_symb_to_end) == 2)) {

        is_const = true;
        is_reg = true;
        is_mem = true;
    }
    else if (sscanf(args, " [%cx]%n", &reg, &count_symb_to_end) == 1 && 
             sscanf(args, " [%lld]%n", &tmp_const, &count_symb_to_end) == 0) {

        is_reg = true;
        is_mem = true;
    }
    else if (sscanf(args, " [%lf]%n", &arg_const, &count_symb_to_end) == 1 &&
        sscanf(args, " [%lld]%n", &tmp_const, &count_symb_to_end) == 1 &&
        (int)arg_const == tmp_const) {

        is_const = true;
        is_mem = true;
    }
    else if ((sscanf(args, " %cx+%lf%n", &reg, &arg_const, &count_symb_to_end) == 2 ||
        sscanf(args, " %lf+%1cx%n", &arg_const, &reg, &count_symb_to_end) == 2) &&
        !is_leftside_arg) {

        is_const = true;
        is_reg = true;
    }
    else if (sscanf(args, " %cx%n", &reg, &count_symb_to_end)      == 1 && 
             sscanf(args, " $%s$%n", str_arg, &count_symb_to_end) == 0 && count_symb_to_end == 2) {

        is_reg = true;
    }
    else if (sscanf(args, " %lf%n", &arg_const, &count_symb_to_end) == 1 && !is_leftside_arg) {
        is_const = true;
    }
    else if (sscanf(args, "$%[^\n]$", str_arg) == 1) {
        if (str_arg[strlen(str_arg) - 1] != '$' && strchr(str_arg, '$') == nullptr) {
            LogSyntaxiceAssembler("Failed: Cannot find end of str", str_arg, num_line);
            return -1;
        } else if (str_arg[strlen(str_arg) - 1] != '$' && strchr(str_arg, '$') != nullptr) {
            LogSyntaxiceAssembler("Failed: Incorrect end of str", str_arg, num_line);
            return -1;
        }
        commands_arr[(*ip)++] = '$';
        for (size_t cur_symb_str = 0; cur_symb_str < strlen(str_arg); ++cur_symb_str, ++(*ip)) {
            commands_arr[(*ip)] = str_arg[cur_symb_str];
        }

        return 0;
    }
    else if (sscanf(args, " %s", arg_label) == 1 &&
        arg_label[0] != '[' && arg_label[strlen(arg_label) - 1] != ']' && !is_leftside_arg) {

        pos = FindPosLabel(arg_label, *count_labels, labels_arr);
        if (pos != -1) {
            (*pointer_to_command) += (char)(CONST_ARG_CMD);
            *((CPU_ARG_INT_T*)(commands_arr + *ip)) = pos;
            (*ip) += sizeof(CPU_ARG_INT_T);
        }
        else if (step == 1) {
            strcpy(labels_arr[*count_labels].name_label, arg_label);
            labels_arr[*count_labels].pos = -1;
            ++(*count_labels);
            (*pointer_to_command) += (char)(CONST_ARG_CMD);
            *((CPU_ARG_INT_T*)(commands_arr + *ip)) = 0;
            (*ip) += sizeof(CPU_ARG_INT_T);
        }
        else {
            LogSyntaxiceAssembler("Failed: Cannot find label", arg_label, -1);
            return -1;
        }

        return 0;
    }
    else {
        LogSyntaxiceAssembler("Failed: Cannot find argument to command", cmd, num_line);
        return -1;
    }

    if (is_const) (*pointer_to_command) += (char)(CONST_ARG_CMD);
    if (is_reg)   (*pointer_to_command) += (char)(REG_ARG_CMD);
    if (is_mem)   (*pointer_to_command) += (char)(RAM_ARG_CMD);

    if (is_reg) {
        if (IsCorrectRegister(reg)) {
            commands_arr[(*ip)++] = reg;
        }
        else {
            LogSyntaxiceAssembler("Failed: Incorrect name of register(correct ax, bx, cx, dx)", cmd, num_line);
            return -1;
        }
    }
    if (is_const) {
        *((CPU_ARG_INT_T*)(commands_arr + (*ip))) = (CPU_ARG_INT_T)(arg_const * PRECISION);
        (*ip) += sizeof(CPU_ARG_INT_T);
    }

    return 0;
}

bool IsCorrectRegister(char reg) {
    if (reg == 'a' || reg == 'b' || reg == 'c' || reg == 'd') {
        return true;
    }
    return false;
}

int FindPosLabel__(const char* label, size_t count_labels, LABEL_* labels_arr, LOCATION_STRUCT_CALL__ call_place) {
    if (label == nullptr) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(label));
        return -1;
    }
    if (_IsBadReadPtr(labels_arr)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(labels_arr));
        return -1;
    }

    for (size_t cur_label = 0; cur_label < count_labels; ++cur_label) {
        if (strcmp(labels_arr[cur_label].name_label, label) == 0) {
            return labels_arr[cur_label].pos;
        }
    }
    return -1;
}

int SetPosLabel__(char* label, int num_line, size_t ip, size_t* count_labels, LABEL_* labels_arr, LOCATION_STRUCT_CALL__ call_place) {
    if (label == nullptr) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(label));
        return -1;
    }
    if (_IsBadReadPtr(count_labels)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(count_labels));
        return -1;
    }
    if (_IsBadReadPtr(labels_arr)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(labels_arr));
        return -1;
    }

    for (size_t cur_label = 0; cur_label < *count_labels; ++cur_label) {
        if (strcmp(labels_arr[cur_label].name_label, label) == 0) {
            if (labels_arr[cur_label].pos != -1) {
                LogSyntaxiceAssembler("Failed: Repeat label", label, num_line);
            }
            labels_arr[cur_label].pos = (int)ip;
            return 0;
        }
    }
    strcpy(labels_arr[*count_labels].name_label, label);
    labels_arr[*count_labels].pos = (int)ip;
    ++(*count_labels);

    return 0;
}

static int LogSyntaxiceAssembler(const char* err_syntax, char* asm_str, int asm_line) {
    fprintf(stderr, "Assembler detect syntaxis error: %s. Command: %s. Line: %d\n", err_syntax, asm_str, asm_line + 1);

    FILE* file_log = fopen(LOG_SYNTAX_FILE_NAME, "a");
    if (_IsBadReadPtr(file_log)) {
        printf("Assembler has error: Cant open log file\n");
        return -1;
    }

    setvbuf(file_log, (char*)NULL, _IONBF, 0);
    fprintf(file_log, "Assembler detect syntaxis error: %s. Command: %s. Line: %d\n", err_syntax, asm_str, asm_line + 1);

    fclose(file_log);

    return 0;
}

int WriteAssemblerToFile__(char* code_arr, size_t size, const char* filename_assembler_text, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(code_arr)) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(code_arr));
        return -1;
    }
    if (filename_assembler_text == nullptr) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(filename_assembler_text));
        return -1;
    }

    FILE* file_assembler_text = fopen(filename_assembler_text, "wb");
    if (filename_assembler_text == nullptr) {
        CreateLog(NAME_MODULE_ASM, "Function get bad ptr", call_place, LOCATION_VAR__(filename_assembler_text));
        return -1;
    }

    HEADER_ASM_FILE header = {};
    HEADER_INIT(header, size);

    fwrite(&header, sizeof(header), 1, file_assembler_text);

    fwrite(code_arr, sizeof(char), size, file_assembler_text);

    fclose(file_assembler_text);

    return 0;
}