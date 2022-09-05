#ifndef BASE_LIB_H__
#define BASE_LIB_H__

/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>

#ifdef __linux__ 
#include <unistd.h>
#include <fcntl.h>
#include <cstddef>
#elif _WIN32
#include <windows.h>
#include <sys\stat.h>
#endif

/*--------------------------------------------------------------------------*/

#define LOCATION__ __FILE__, __func__, __LINE__
#define LOCATION_VAR__(var) #var, __FILE__, __func__, __LINE__
#define COMPLETE_STR__(...) __VA_ARGS__
#define NAME_VAR_TO_STR(var) #var
#define COMPLETE_STR_VAR__(str, var) str #var

struct LOCATION_STRUCT_CALL__ {
    const char* filename__  = "";
    const char* func_name__ = "";
    int line__ = -1;
};

struct LOCATION_VAR_STRUCT__ {
    const char* var_name__  = "";
    const char* filename__  = "";
    const char* func_name__ = "";
    int line__              = -1;
};

/*--------------------------------------------------------------------------*/

#ifdef __linux__
    #define _IsBadReadPtr(pointer) _IsBadReadPtr__(pointer, sizeof(pointer))    
    bool _IsBadReadPtr__(void* pointer, size_t size);
#elif _WIN32
    #define _IsBadReadPtr(pointer) _IsBadReadPtr__(pointer)
    bool _IsBadReadPtr__(void* pointer);
#endif

/*--------------------------------------------------------------------------*/


#ifdef _WIN32
    int file_cmp(const char* file1, const char* file2);
#endif

/*--------------------------------------------------------------------------*/

long long getFileSize(FILE* file/*!< - Pointer to opened file*/);

#endif