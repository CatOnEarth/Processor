#ifndef TEXT_LIB_H__
#define TEXT_LIB_H__

/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include "../base_lib/base_lib.h"

/*--------------------------------------------------------------------------*/

struct StringStruct {
    char*  str    = nullptr;
    size_t lenght = 0;
};

#define SwapStr(left, right) SwapStr__(left, right, LOCATION_STRUCT_CALL__{LOCATION__})
int SwapStr__(StringStruct* left, StringStruct* right, LOCATION_STRUCT_CALL__ second_var);

/*--------------------------------------------------------------------------*/

struct TextStruct {
    StringStruct  text = {};
    StringStruct* strings_text = nullptr;
    int           num_strings  = 0;
};

#define   TextConstructor(text)   TextConstructor__(text, LOCATION_STRUCT_CALL__{LOCATION__})
int       TextConstructor__       (TextStruct* text, LOCATION_STRUCT_CALL__ call_place);

#define   TextDestructor(text)    TextDestructor__(text, LOCATION_STRUCT_CALL__{LOCATION__})
int       TextDestructor__        (TextStruct* text, LOCATION_STRUCT_CALL__ call_place);

#define   GetMemoryForTextBuf(programm_code_text, file_code_text) GetMemoryForTextBuf__(programm_code_text, file_code_text,   \
                              LOCATION_STRUCT_CALL__{LOCATION__})
int       GetMemoryForTextBuf__   (TextStruct* programm_code_text, FILE* file_code_text, LOCATION_STRUCT_CALL__ call_place);

#define   CountNumOfStrings(str_input) CountNumOfStrings__(str_input, LOCATION_STRUCT_CALL__{LOCATION__})
int       CountNumOfStrings__     (StringStruct* str_input, LOCATION_STRUCT_CALL__ call_place);

#define   EraseSpaceStartEndStr(str_input) EraseSpaceStartEndStr__(str_input, LOCATION_STRUCT_CALL__{LOCATION__})
int       EraseSpaceStartEndStr__ (StringStruct* str_input, LOCATION_STRUCT_CALL__ call_place);

#endif