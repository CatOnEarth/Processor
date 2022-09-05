#include "logger.h"
#include "../base_lib/base_lib.h"
#include <time.h> 

static const char* LOG_FILE_NAME = "log.txt";

int CreateLog(const char* name_module, const char* err_text, LOCATION_STRUCT_CALL__ call_place, 
                     const char* var_name, const char* filename, const char* funcname, int line) {

    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    fprintf(stderr, "%s  :  %s has error: %s. WHERE: FILENAME: %s;FUNCTION: %s;LINE: %d. Error was detecte in: FILENAME: %s;FUNCTION: %s;LINE: %d;VAR: %s\n",
        asctime(timeinfo), name_module, err_text, call_place.filename__, call_place.func_name__, call_place.line__,
        filename, funcname, line, var_name);

    FILE* file_log = fopen(LOG_FILE_NAME, "w+");
    if (_IsBadReadPtr(file_log)) {
        printf("%s  :  %s has error: Cant open log file\n", asctime(timeinfo), name_module);
        return -1;
    }

    setvbuf(file_log, (char*)NULL, _IONBF, 0);
    fprintf(file_log, "%s  :  %s has error: %s. WHERE: FILENAME: %s;FUNCTION: %s;LINE: %d. Error was detecte in: FILENAME: %s;FUNCTION: %s;LINE: %d;VAR: %s\n",
        asctime(timeinfo), name_module, err_text, call_place.filename__, call_place.func_name__, call_place.line__,
        filename, funcname, line, var_name);

    fclose(file_log);

    return 0;
}
