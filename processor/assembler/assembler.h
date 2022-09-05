#ifndef ASSEMBLER_H__
#define ASSEMBLER_H__

/*----------------------------------------*/

#include "../text_lib/text_lib.h"

/*----------------------------------------*/

#define CreateAssemblerFile(filename_code, filename_asm) CreateAssemblerFile__(filename_code, filename_asm, LOCATION_STRUCT_CALL__{LOCATION__}) 
int CreateAssemblerFile__(const char* filename_code, const char* filename_asm, LOCATION_STRUCT_CALL__ call_place);

#endif