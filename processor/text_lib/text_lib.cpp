#include "text_lib.h"

static int LogTextLib(const char* err_text, LOCATION_STRUCT_CALL__ origin_var, const char* var_name, const char* filename, const char* funcname, int line);

const char* LOG_FILE_NAME = "log_text_lib.txt";

/*!
    * Function for swap two StringStruct elems
*/
int SwapStr__(StringStruct* left, StringStruct* right, LOCATION_STRUCT_CALL__ call_place) {

    if (_IsBadReadPtr(left)) {
        LogTextLib("Function get bad ptr", call_place, LOCATION_VAR__(left));
        return -1;
    }
    if (_IsBadReadPtr(right)) {
        LogTextLib("Function get bad ptr", call_place, LOCATION_VAR__(right));
        return -1;
    }

    StringStruct tmp = *left;
    *left            = *right;
    *right           = tmp;

    return 0;
}

/**
    * Constructor default
*/
int TextConstructor__(TextStruct* text, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(text)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(text));
        return -1;
    }

    if (!(text->num_strings  == 0 &&
          text->strings_text == nullptr)) {

        LogTextLib("Repeat constructor detected ", call_place, LOCATION_VAR__(text));
        return -1;
    }

    text->num_strings  = 0;
    text->strings_text = nullptr;

    return 0;
}

/**
    * Destructor
*/
int TextDestructor__(TextStruct* text, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(text)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(text));
        return -1;
    }

    if (_IsBadReadPtr(text->strings_text)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(text->strings_text));
        return -1;
    }

    if (_IsBadReadPtr(text->text.str)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(text->text.str));
        return -1;
    }

    free(text->strings_text);
    free(text->text.str);

    return 0;
}

int GetMemoryForTextBuf__(TextStruct* programm_code_text, FILE* file_code_text, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(programm_code_text)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(programm_code_text));
        return -1;
    }
    if (_IsBadReadPtr(file_code_text)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(text));
        return -1;
    }

    programm_code_text->text.str = (char*)calloc(getFileSize(file_code_text) + 1, sizeof(char));
    if (_IsBadReadPtr(programm_code_text->text.str)) {
        LogTextLib("Cant calloc memory ", LOCATION_STRUCT_CALL__(), LOCATION_VAR__(text));
        return -1;
    }

    return 0;
}

/**
    * Function to count strings in buffer of text code
*/
int CountNumOfStrings__(StringStruct* str_input, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(str_input)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(str_input));
        return -1;
    }
    if (_IsBadReadPtr(str_input->str)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(str_input->str));
        return -1;
    }

    int num_of_strings = 0;
    for (size_t cur_symb = 0; cur_symb < strlen(str_input->str); ++cur_symb) {
        if (str_input->str[cur_symb] == '\n' || str_input->str[cur_symb] == '\r') {
            ++num_of_strings;
        }
    }

    return num_of_strings + 1;
}

int EraseSpaceStartEndStr__(StringStruct* str_input, LOCATION_STRUCT_CALL__ call_place) {
    if (_IsBadReadPtr(str_input)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(str_input));
        return -1;
    }
    if (_IsBadReadPtr(str_input->str)) {
        LogTextLib("Function get bad pointer ", call_place, LOCATION_VAR__(str_input->str));
        return -1;
    }

    for (size_t start_symb = 0, end_symb = str_input->lenght - 1; start_symb < end_symb;) {
        if (str_input->str[start_symb] == ' ') {
            str_input->str[start_symb] = '\0';
            ++str_input->str;
            --str_input->lenght;
            ++start_symb;
            continue;
        }
        if (str_input->str[end_symb] == ' ') {
            str_input->str[end_symb] = '\0';
            --str_input->lenght;
            --end_symb;
            continue;
        }

        break;
    }

    return 0;
}

static int LogTextLib(const char* err_text, LOCATION_STRUCT_CALL__ call_place, const char* var_name, const char* filename, const char* funcname, int line) {
    printf ("TextLib has error: %s. WHERE: FILENAME: %s;FUNCTION: %s;LINE: %d. Error was detecte in: FILENAME: %s;FUNCTION: %s;LINE: %d;VAR: %s\n", 
             err_text, call_place.filename__, call_place.func_name__, call_place.line__,
             filename, funcname, line, var_name);
    fprintf(stderr, "TextLib has error: %s. WHERE: FILENAME: %s;FUNCTION: %s;LINE: %d. Error was detecte in: FILENAME: %s;FUNCTION: %s;LINE: %d;VAR: %s\n",
                     err_text, call_place.filename__, call_place.func_name__, call_place.line__,
                     filename, funcname, line, var_name);

    FILE* file_log = fopen(LOG_FILE_NAME, "w+");
    if (_IsBadReadPtr(file_log)) {
        printf("TextLib has error: Cant open log file\n");
        return -1;
    }

    setvbuf(file_log, (char*)NULL, _IONBF, 0);
    fprintf(file_log, "TextLib has error: %s. WHERE: FILENAME: %s;FUNCTION: %s;LINE: %d. Error was detecte in: FILENAME: %s;FUNCTION: %s;LINE: %d;VAR: %s\n",
                      err_text, call_place.filename__, call_place.func_name__, call_place.line__,
                      filename, funcname, line, var_name);

    fclose(file_log);

    return 0;
}
