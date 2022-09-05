#pragma once

/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <typeinfo>
#include <cstddef>

/*--------------------------------------------------------------------------*/

#define DEBUG_MODE_OFF 0
#define DEBUG_MODE_ON  1

#define DEBUG_MODE DEBUG_MODE_OFF

#ifndef DEBUG_MODE
    #define DEBUG_MODE DEBUG_MODE_OFF
#endif

#define STACK_WITHOUT_PROTECTION  0
#define STACK_STAND_PROTECTION    1
#define STACK_CANARY_PROTECTION   2
#define STACK_HASH_PROTECTION     3

#define STACK_LEVEL_PROTECTION     STACK_WITHOUT_PROTECTION

#ifndef STACK_LEVEL_PROTECTION
    #define STACK_LEVEL_PROTECTION STACK_HASH_PROTECTION
#endif

enum class TypeError {
    _SUCCESSFUL,
    _ERROR_NULL_OBJ,
    _ERROR_NULL_POINTER_DATA,
    _ERROR_INIZIALIZE_DATA,
    _ERROR_SEGMENTATION_FAULT,
    _ERROR_SIZE_MINUS,
    _ERROR_CAPACITY_CANT_BE_LOW_DEFAULT,
    _ERROR_SIZE_BIGGER_CAPACITY,
    _ERROR_REPEAT_CONSTRACT,
    _ERROR_REPEAT_DESTRUCT,
    _ERROR_POISON_ELEM_STACK_DATA,
    _ERROR_STACK_NOT_CONSTRACT,
    _ERROR_STACK_DESTRUCT,
    _ERROR_POP_ON_EMPTY_STACK,
    _ERROR_TOP_ON_EMPTY_STACK,
    _ERROR_CANARY_STACK_STRUCT_START_DEAD,
    _ERROR_CANARY_STACK_STRUCT_END_DEAD,
    _ERROR_CANARY_STACK_DATA_START_DEAD,
    _ERROR_CANARY_STACK_DATA_END_DEAD,
    _ERROR_HASH_STRUCT_CHANGED,
    _ERROR_HASH_DATA_CHANGED
};

enum class Poison {
    pNullptr,
    pNullData
};

#if DEBUG_MODE == DEBUG_MODE_ON
struct LOCATION {
    const char* file_name;
    const char* func_name;
    long long   num_line;
    const char* var_type;
    const char* var_name;
};
#endif

typedef long long          StackElem_t;
typedef unsigned long long StackCanaryElem_t;

#define DEFAULT_CAPACITY            16
#define DEFAULT_EMPTY_ELEM_OF_STACK -666

#define STACK_RESIZE_MORE_CONST     16

const unsigned long long CANARY_DEFAULT_STRUCT_START = 0x5C4F2EB5; // 1548693173
const unsigned long long CANARY_DEFAULT_STRUCT_END   = 0x7350C11E; // 1934672158

const unsigned long long CANARY_DEFAULT_DATA_START   = 0x813794E5; // 2167903461
const unsigned long long CANARY_DEFAULT_DATA_END     = 0x7A0254E4; // 2046973156

struct Stack_t {
#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION 
    StackCanaryElem_t canary_start;
#endif

    StackElem_t* data;
    long long    size;
    long long    capacity;

#if DEBUG_MODE == DEBUG_MODE_ON
    LOCATION     location;
#endif

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION 
    long long hash_data;
    long long hash_struct;
#endif

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION 
    StackCanaryElem_t canary_end;
#endif
};

#if DEBUG_MODE == DEBUG_MODE_ON
    #define DEBUG_CODE_ADD(...) __VA_ARGS__
#endif

#if DEBUG_MODE == DEBUG_MODE_OFF
    #define DEBUG_CODE_ADD(...)
#endif

#define StackType

#if DEBUG_MODE == DEBUG_MODE_ON
    #define ASSERT_OK(obj)                                                                \
        if (StackType##OK(obj) != TypeError::_SUCCESSFUL) {                               \
            StackAbort(obj, StackType##OK(obj), LOCATION{ __FILE__, __func__, __LINE__,   \
                       typeid(StackElem_t).name(), #obj });                               \
    }
#endif

#if DEBUG_MODE == DEBUG_MODE_OFF
    #define ASSERT_OK(obj)                                      \
        if (StackType##OK(obj) != TypeError::_SUCCESSFUL) {     \
            StackAbort(obj, StackType##OK(obj));                \
        }
#endif

#if DEBUG_MODE == DEBUG_MODE_ON
    #define StackConstructor(stack) StackConstructor_(&stack, LOCATION {__FILE__, __FUNCTION__, __LINE__, \
                             typeid(StackElem_t).name(), #stack});
#endif

#if DEBUG_MODE == DEBUG_MODE_OFF
    #define StackConstructor(stack) StackConstructor_(&stack);
#endif

/*-------------------------------------------------------------------------*/

TypeError StackConstructor_          (Stack_t* stack DEBUG_CODE_ADD(, LOCATION location_call));
bool      CheckIsWasAlreadyConstract (Stack_t* stack DEBUG_CODE_ADD(, LOCATION location_call));
TypeError StackDestructor            (Stack_t* stack);

/*-------------------------------------------------------------------------*/

TypeError   StackPush (Stack_t* stack, StackElem_t val);
StackElem_t StackPop  (Stack_t* stack);
StackElem_t StackTop  (Stack_t* stack);

/*-------------------------------------------------------------------------*/

TypeError StackDataAllocation       (Stack_t* stack);
void      DataPoisonElemsInizialize (StackElem_t* start, StackElem_t* end);

/*-------------------------------------------------------------------------*/

#if STACK_LEVEL_PROTECTION == STACK_WITHOUT_PROTECTION
    TypeError StackTypeOK               (Stack_t* /*stack*/);
#else
    TypeError StackTypeOK               (Stack_t* stack);
#endif

TypeError StackTypeOKStandartProtection (Stack_t* stack);
TypeError StackTypeOKCanaryProtection   (Stack_t* stack);
TypeError StackTypeOKHashProtection     (Stack_t* stack);

/*-------------------------------------------------------------------------*/

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION 
TypeError HashReCalculate (Stack_t* stack);
long long HashFunc        (void* start_hash, void* end_hash);
#endif

/*-------------------------------------------------------------------------*/

void        StackAbort        (Stack_t* stack, TypeError err_ DEBUG_CODE_ADD(, LOCATION location_call));
const char* StackGetTextError (TypeError err);
void        StackDump         (Stack_t* stack, TypeError err_ DEBUG_CODE_ADD(, LOCATION location_call));
