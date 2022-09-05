#ifndef LOGGER_H__
#define LOGGER_H__

/*----------------------------------------*/

#include "../base_lib/base_lib.h"

/*----------------------------------------*/

int CreateLog(const char* name_module, const char* err_text, LOCATION_STRUCT_CALL__ call_place,
    const char* var_name, const char* filename, const char* funcname, int line);

#endif