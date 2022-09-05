#include "stack_lib.h"
#include "../../base_lib/base_lib.h"

TypeError StackConstructor_(Stack_t* stack DEBUG_CODE_ADD(, LOCATION location_call)) {
    if (_IsBadReadPtr(stack)) {
        StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, location_call));
    }

    DEBUG_CODE_ADD(stack->location = {};)

    if (CheckIsWasAlreadyConstract(stack DEBUG_CODE_ADD(, location_call))) {
        StackAbort(stack, TypeError::_ERROR_REPEAT_CONSTRACT DEBUG_CODE_ADD(, location_call));
    }

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION 
    stack->canary_start = CANARY_DEFAULT_STRUCT_START;
    stack->canary_end   = CANARY_DEFAULT_STRUCT_END;
#endif

    DEBUG_CODE_ADD(stack->location = location_call;)

    stack->capacity = DEFAULT_CAPACITY;
    stack->size     = 0;

    stack->data = nullptr;

    StackDataAllocation(stack);

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION 
    stack->hash_data   = 0;
    stack->hash_struct = 0;

    stack->hash_struct = HashFunc(stack,       (char*)stack + sizeof(Stack_t));
    stack->hash_data   = HashFunc(stack->data, stack->data + stack->capacity);
#endif

    return TypeError::_SUCCESSFUL;
}

bool CheckIsWasAlreadyConstract(Stack_t* stack DEBUG_CODE_ADD(, LOCATION location_call)) {
    if (_IsBadReadPtr(stack)) {
        StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, location_call));
    }

    if (stack->data     == nullptr &&
        stack->size     == 0       &&
        stack->capacity == 0       ) {

#if STACK_LEVEL_PROTECTION == STACK_CANARY_PROTECTION
        if (stack->canary_start == 0 &&
            stack->canary_end == 0) {

            return false;
        }
#elif STACK_LEVEL_PROTECTION == STACK_HASH_PROTECTION
        if (stack->canary_start == 0 &&
            stack->canary_end   == 0 &&
            stack->hash_struct  == 0 &&
            stack->hash_data    == 0) {

            return false;
        }
#else
        return false;
#endif
    }

    return true;
}

TypeError StackDestructor(Stack_t* stack) {
    ASSERT_OK(stack);

    DataPoisonElemsInizialize(stack->data, stack->data + stack->capacity);

#if STACK_LEVEL_PROTECTION < STACK_CANARY_PROTECTION
    free(stack->data);
#endif

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
    free((char*)stack->data - sizeof(StackCanaryElem_t));

    stack->canary_start = -1;
    stack->canary_end   = -1;
#endif

    stack->data = (StackElem_t*)Poison::pNullData;

    stack->capacity = -1;
    stack->size     = -1;

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    stack->hash_data   = -1;
    stack->hash_struct = -1;
#endif

    return TypeError::_SUCCESSFUL;
}

TypeError StackPush(Stack_t* stack, StackElem_t val) {
    ASSERT_OK(stack);

    stack->data[stack->size] = val;
    ++stack->size;

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    HashReCalculate(stack);
#endif

    if (stack->size == stack->capacity) {
        StackDataAllocation(stack);
    }

    ASSERT_OK(stack);

    return TypeError::_SUCCESSFUL;
}

StackElem_t StackPop(Stack_t* stack) {
    ASSERT_OK(stack);

    if (stack->size - 1 < 0) {
        StackAbort(stack, TypeError::_ERROR_POP_ON_EMPTY_STACK DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                    stack->location.var_type, stack->location.var_name }));
    }

    StackElem_t prev_val = stack->data[stack->size - 1];
    stack->data[stack->size - 1] = DEFAULT_EMPTY_ELEM_OF_STACK;
    --stack->size;

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    HashReCalculate(stack);
#endif
    if (stack->capacity == stack->size + STACK_RESIZE_MORE_CONST * 2) {
        StackDataAllocation(stack);
    }

    ASSERT_OK(stack);

    return prev_val;
}

StackElem_t StackTop(Stack_t* stack) {
    ASSERT_OK(stack);

    if (stack->size - 1 < 0) {
        StackAbort(stack, TypeError::_ERROR_TOP_ON_EMPTY_STACK DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                   stack->location.var_type, stack->location.var_name }));
    }

    return stack->data[stack->size - 1];
}

TypeError StackDataAllocation(Stack_t* stack) {
    if (_IsBadReadPtr(stack)) {
        StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                   typeid(StackElem_t).name(), "_ERROR__" }));
    }

    long long capacity_old         = -1;
#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    bool is_need_hash_recalculate  = false;
    bool is_need_move_pointer_data = false;
#endif

    if (stack->size == stack->capacity) {   // UP
        ASSERT_OK(stack);
        capacity_old = stack->capacity;
        stack->capacity += STACK_RESIZE_MORE_CONST;
#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
        is_need_hash_recalculate  = true;
        is_need_move_pointer_data = true;
#endif
    } else if (stack->capacity - STACK_RESIZE_MORE_CONST * 2 >= stack->size) {  // DOWN
        ASSERT_OK(stack);
        stack->capacity -= STACK_RESIZE_MORE_CONST;
#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
        is_need_hash_recalculate  = true;
        is_need_move_pointer_data = true;
#endif
    } else if (stack->size == 0 && stack->capacity == DEFAULT_CAPACITY && stack->data ==  nullptr) {   // INIZIALIZE
        if (stack == nullptr) {
            StackAbort(stack, TypeError::_ERROR_NULL_OBJ DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                       typeid(StackElem_t).name(), "nullptr" }));
        }
        if (_IsBadReadPtr(stack)) {
            StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                       typeid(StackElem_t).name(), "_ERROR__" }));
        }
    }
    else {
        return TypeError::_SUCCESSFUL;
    }

#if STACK_LEVEL_PROTECTION < STACK_CANARY_PROTECTION
    long long new_size = stack->capacity * sizeof(StackElem_t);

    stack->data = (StackElem_t*)realloc(stack->data, new_size);

    if (stack->data == nullptr) {
        StackAbort(stack, TypeError::_ERROR_NULL_POINTER_DATA DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                   typeid(StackElem_t).name(), stack->location.var_name }));
    }
#endif

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
    long long new_size = stack->capacity * sizeof(StackElem_t) + 2 * sizeof(StackCanaryElem_t);

    if (is_need_move_pointer_data) {
        stack->data = (StackElem_t*)((char*)stack->data - sizeof(StackCanaryElem_t));
    }

    stack->data = (StackElem_t*)((char*)realloc((char*)stack->data, new_size));

    if (stack->data == nullptr) {
        StackAbort(stack, TypeError::_ERROR_NULL_POINTER_DATA DEBUG_CODE_ADD(, LOCATION{ __FILE__, __FUNCTION__, __LINE__,
                   typeid(StackElem_t).name(), stack->location.var_name }));
    }

    *((StackCanaryElem_t*) (char*)stack->data) = CANARY_DEFAULT_DATA_START;

    stack->data = (StackElem_t*) ((char*)stack->data + sizeof(StackCanaryElem_t));

    *((StackCanaryElem_t*) (stack->data + stack->capacity)) = CANARY_DEFAULT_DATA_END;
#endif

    if (capacity_old != -1) {
        DataPoisonElemsInizialize(stack->data + capacity_old, stack->data + stack->capacity);
    }

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    if (is_need_hash_recalculate) {
        HashReCalculate(stack);
    }
#endif

    return TypeError::_SUCCESSFUL;
}

void DataPoisonElemsInizialize(StackElem_t* start, StackElem_t* end) {
    if (end - start < 0) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __FUNCTION__, __LINE__,
                           typeid(StackElem_t).name(), "end - start < 0" }));
    }

    size_t lenght = end - start;

    for (size_t num_elem = 0; num_elem < lenght; ++num_elem) {
        if (_IsBadReadPtr(start + num_elem)) {
            StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __FUNCTION__, __LINE__,
                       typeid(StackElem_t).name(), "start + num_elem" }));
        }
        *(start + num_elem) = DEFAULT_EMPTY_ELEM_OF_STACK;
    }
}

#if STACK_LEVEL_PROTECTION == STACK_WITHOUT_PROTECTION
TypeError StackTypeOK(Stack_t* /*stack*/) { return TypeError::_SUCCESSFUL; }
#else
TypeError StackTypeOK(Stack_t* stack) {
#if STACK_LEVEL_PROTECTION == STACK_WITHOUT_PROTECTION
    return TypeError::_SUCCESSFUL;
#endif

#if STACK_LEVEL_PROTECTION == STACK_STAND_PROTECTION
    return StackTypeOKStandartProtection(stack);
#endif

#if STACK_LEVEL_PROTECTION == STACK_CANARY_PROTECTION
    return StackTypeOKCanaryProtection(stack);
#endif // STACK_LEVEL_PROTECTION

#if STACK_LEVEL_PROTECTION == STACK_HASH_PROTECTION
    return StackTypeOKHashProtection(stack);
#endif // STACK_LEVEL_PROTECTION
}
#endif

#if STACK_LEVEL_PROTECTION >= STACK_STAND_PROTECTION
TypeError StackTypeOKStandartProtection(Stack_t* stack) {
    if (_IsBadReadPtr(stack)) {
        return TypeError::_ERROR_SEGMENTATION_FAULT;
    }
    if (stack == nullptr) {
        return TypeError::_ERROR_NULL_OBJ;
    }
    if (stack->data == nullptr) {
        return TypeError::_ERROR_NULL_POINTER_DATA;
    }
    if (stack->data == (StackElem_t*) Poison::pNullData) {
        return TypeError::_ERROR_STACK_DESTRUCT;
    }
    if (_IsBadReadPtr(stack->data)) {
        return TypeError::_ERROR_SEGMENTATION_FAULT;
    }
    if (stack->size < 0) {
        return TypeError::_ERROR_SIZE_MINUS;
    }
    if (stack->capacity < DEFAULT_CAPACITY) {
        return TypeError::_ERROR_CAPACITY_CANT_BE_LOW_DEFAULT;
    }
    if (stack->capacity < stack->size) {
        return TypeError::_ERROR_SIZE_BIGGER_CAPACITY;
    }

    return TypeError::_SUCCESSFUL;
}
#endif

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
TypeError StackTypeOKCanaryProtection(Stack_t* stack) {
    TypeError err = StackTypeOKStandartProtection(stack);
    if (err != TypeError::_SUCCESSFUL) {
        StackAbort(stack, err DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__, typeid(StackElem_t).name(), "stack" }));
    }

    if (stack->canary_start != CANARY_DEFAULT_STRUCT_START) {
        return TypeError::_ERROR_CANARY_STACK_STRUCT_START_DEAD;
    }
    if (stack->canary_end   != CANARY_DEFAULT_STRUCT_END) {
        return TypeError::_ERROR_CANARY_STACK_STRUCT_END_DEAD;
    }
    if (_IsBadReadPtr((StackCanaryElem_t*)((char*)stack->data - sizeof(StackCanaryElem_t)))) {
        return TypeError::_ERROR_SEGMENTATION_FAULT;
    }
    if (*((StackCanaryElem_t*) ((char*)stack->data - sizeof(StackCanaryElem_t))) != CANARY_DEFAULT_DATA_START) {
        return TypeError::_ERROR_CANARY_STACK_DATA_START_DEAD;
    }
    if (_IsBadReadPtr((StackCanaryElem_t*)(stack->data + stack->capacity))) {
        return TypeError::_ERROR_SEGMENTATION_FAULT;
    }
    if (*((StackCanaryElem_t*) (stack->data + stack->capacity))                  != CANARY_DEFAULT_DATA_END) {
        return TypeError::_ERROR_CANARY_STACK_DATA_END_DEAD;
    }

    return TypeError::_SUCCESSFUL;
}
#endif

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
TypeError StackTypeOKHashProtection(Stack_t* stack) {
    TypeError err = StackTypeOKCanaryProtection(stack);
    if (err != TypeError::_SUCCESSFUL) {
        StackAbort(stack, err DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__, typeid(StackElem_t).name(), "stack" }));
    }

    long long saved_hash_struct = stack->hash_struct;
    long long saved_hash_data   = stack->hash_data;

    stack->hash_struct = 0;
    stack->hash_data   = 0;

    long long cur_hash_struct = HashFunc(stack      , (char*)stack + sizeof(Stack_t));
    long long cur_hash_data   = HashFunc(stack->data, stack->data + stack->capacity);

    stack->hash_struct = saved_hash_struct;
    stack->hash_data   = saved_hash_data;

    if (cur_hash_struct != saved_hash_struct) {
        return TypeError::_ERROR_HASH_STRUCT_CHANGED;
    }
    if (cur_hash_data != saved_hash_data) {
        return TypeError::_ERROR_HASH_DATA_CHANGED;
    }

    for (int num_ind = 0; num_ind < stack->size; ++num_ind) {
        if (stack->data[num_ind] == DEFAULT_EMPTY_ELEM_OF_STACK) {
            return TypeError::_ERROR_POISON_ELEM_STACK_DATA;
        }
    }

    return TypeError::_SUCCESSFUL;
}
#endif

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
TypeError HashReCalculate(Stack_t* stack) {
    if (_IsBadReadPtr(stack)) {
        StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                   typeid(StackElem_t).name(), "stack" }));
    }

    TypeError err = TypeError::_SUCCESSFUL;

#if STACK_LEVEL_PROTECTION == STACK_STAND_PROTECTION
    err = StackTypeOKStandartProtection(stack);
#elif STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
    err = StackTypeOKCanaryProtection(stack);
#endif

    if (err != TypeError::_SUCCESSFUL) {
        StackDump(stack, TypeError::_ERROR_INIZIALIZE_DATA DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                  stack->location.var_type, "stack" }));
        abort();
    }

    stack->hash_data   = 0;
    stack->hash_struct = 0;

    long long cur_hash_data = HashFunc(stack->data, stack->data + stack->capacity);
    stack->hash_struct      = HashFunc(stack, (char*)stack + sizeof(Stack_t));
    stack->hash_data        = cur_hash_data;

    return TypeError::_SUCCESSFUL;
}

/* Hash function djb2 http://www.cse.yorku.ca/~oz/hash.html */
long long HashFunc(void* start_hash, void* end_hash) {
    if (_IsBadReadPtr(start_hash)) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __FUNCTION__, __LINE__,
                   "void*", "start_hash" }));
    }
    if (_IsBadReadPtr(end_hash)) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __func__, __LINE__,
                   "void*", "end_hash" }));
    }

    if (!(start_hash < end_hash)) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __FUNCTION__, __LINE__,
                   "void*", "start_hash, end_hash" }));
    }

    long long res_hash_func = 0;
    for (size_t num_byte = 0; num_byte < (char*)end_hash - (char*)start_hash; ++num_byte) {
        if (_IsBadReadPtr((char*)start_hash + num_byte)) {
            StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE_ADD(, LOCATION{ __FILE__, __FUNCTION__, __LINE__,
                       "void*", "start_hash" }));
        }
        char byte_value = *((char*)start_hash + num_byte);
        res_hash_func = ((res_hash_func << 5) + res_hash_func) + byte_value;
    }

    return res_hash_func;
}
#endif

void StackAbort(Stack_t* stack, TypeError err_ DEBUG_CODE_ADD(, LOCATION location_call)) {
    StackDump(stack, err_ DEBUG_CODE_ADD(, location_call));
    return;
}

const char* StackGetTextError(TypeError err) {
    const char* text_err = 0;
    switch (err) {
    case TypeError::_SUCCESSFUL:
        text_err = "Something wrong";
        break;
    case TypeError::_ERROR_NULL_OBJ:
        text_err = "Param of stack was nullptr";
        break;
    case TypeError::_ERROR_NULL_POINTER_DATA:
        text_err = "Data in stack was nullptr";
        break;
    case TypeError::_ERROR_INIZIALIZE_DATA:
        text_err = "Data in stack inizialize failed";
        break;
    case TypeError::_ERROR_SEGMENTATION_FAULT:
        text_err = "Error in pointers";
        break;
    case TypeError::_ERROR_SIZE_MINUS:
        text_err = "Size of stack is low than zero";
        break;
    case TypeError::_ERROR_CAPACITY_CANT_BE_LOW_DEFAULT:
        text_err = "Capacity of stack is low than default";
        break;
    case TypeError::_ERROR_SIZE_BIGGER_CAPACITY:
        text_err = "Size of stack is bigger than capacity of stack";
        break;
    case TypeError::_ERROR_REPEAT_CONSTRACT:
        text_err = "Calling the stack constructor again";
        break;
    case TypeError::_ERROR_REPEAT_DESTRUCT:
        text_err = "Calling the stack destructor again";
        break;
    case TypeError::_ERROR_POISON_ELEM_STACK_DATA:
        text_err = "Element of stack was default poisen Element";
        break;
    case TypeError::_ERROR_STACK_NOT_CONSTRACT:
        text_err = "Stack was not constract";
        break;
    case TypeError::_ERROR_STACK_DESTRUCT:
        text_err = "Stack was already destruct";
        break;
    case TypeError::_ERROR_POP_ON_EMPTY_STACK:
        text_err = "Try to pop on empty stack";
        break;
    case TypeError::_ERROR_TOP_ON_EMPTY_STACK:
        text_err = "Try to top on empty stack";
        break;
    case TypeError::_ERROR_CANARY_STACK_STRUCT_START_DEAD:
        text_err = "Canary of start struct stack was changed";
        break;
    case TypeError::_ERROR_CANARY_STACK_STRUCT_END_DEAD:
        text_err = "Canary of end struct stack was changed";
        break;
    case TypeError::_ERROR_CANARY_STACK_DATA_START_DEAD:
        text_err = "Canary of start data stack was changed";
        break;
    case TypeError::_ERROR_CANARY_STACK_DATA_END_DEAD:
        text_err = "Canary of end data stack was changed";
        break;
    case TypeError::_ERROR_HASH_STRUCT_CHANGED:
        text_err = "Hash struct stack was changed";
        break;
    case TypeError::_ERROR_HASH_DATA_CHANGED:
        text_err = "Hash data stack was changed";
        break;
    default:
        text_err = "Something wrong";
        break;
    }

    return text_err;
}

void StackDump(Stack_t* stack, TypeError err_ DEBUG_CODE_ADD(, LOCATION location_call)) {
    FILE* dump_file = fopen("dump_stack.txt", "w");
    assert(dump_file != nullptr);

    setvbuf(dump_file, (char*)NULL, _IONBF, 0);

    const char* text_err = StackGetTextError(err_);

    printf("\n%s\n", text_err);

    fprintf(dump_file, "\n---------------------------------\n");
  //  fprintf(dump_file, "\nCurrent time: %s\n", ctime(&cur_time));
    fprintf(dump_file, "\nTime of last build: %s  %s\n", __DATE__, __TIME__);
    fprintf(dump_file, "\nError: %s\n", text_err);

    if (!_IsBadReadPtr(stack)) {
#if DEBUG_MODE == DEBUG_MODE_ON
        if (CheckIsWasAlreadyConstract(stack, location_call)) {
            fprintf(dump_file, "\nDeclareded variable: %s<%s> [0x%x] in File: %s   in Function: %s    in Line: %d\n",
                stack->location.var_name, stack->location.var_type,
                stack, stack->location.file_name, stack->location.func_name, stack->location.num_line);
        }

        fprintf(dump_file, "\nNow variable: %s<%s> [0x%x] in File: %s   in Function: %s    in Line: %d\n",
            location_call.var_name, location_call.var_type, stack, 
            location_call.file_name, location_call.func_name, location_call.num_line);
#endif

        fprintf(dump_file, "\tcapacity: %lld\n", stack->capacity);
        fprintf(dump_file, "\tsize:     %lld\n", stack->size);

        fprintf(dump_file, "\n");

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
        fprintf(dump_file, "\tcanary_start_struct:         %u\n", stack->canary_start);
        fprintf(dump_file, "\tdefault canary_start_struct: %u\n", CANARY_DEFAULT_STRUCT_START);

        fprintf(dump_file, "\n");

        fprintf(dump_file, "\tcanary_end_struct:           %u\n", stack->canary_end);
        fprintf(dump_file, "\tdefault canary_end_struct:   %u\n", CANARY_DEFAULT_STRUCT_END);

        fprintf(dump_file, "\n");
#endif

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
        fprintf(dump_file, "\thash_struct_last_changed:    %d\n", stack->hash_struct);
        fprintf(dump_file, "\thash_struct_now:             %d\n", HashFunc(stack, (char*)stack + sizeof(Stack_t)));

        fprintf(dump_file, "\n");
#endif

        if (!_IsBadReadPtr(stack->data)) {
#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
            fprintf(dump_file, "\thash_data_last_changed:      %d\n", stack->hash_data);
            fprintf(dump_file, "\thash_data_now:               %d\n", HashFunc(stack->data, stack->data + stack->capacity));

            fprintf(dump_file, "\n");
#endif
            fprintf(dump_file, "\tdata[0x%llx] {\n", (unsigned long long)stack->data);

            for (long long num_elem = 0; num_elem < stack->capacity; ++num_elem) {
                if (!_IsBadReadPtr(stack->data + num_elem)) {
                    if (num_elem < stack->size) {
                        fprintf(dump_file, "\t\t* [%lld] = %lld\n", num_elem, stack->data[num_elem]);
                    }
                    else {
                        fprintf(dump_file, "\t\t [%lld] = %lld\n", num_elem, stack->data[num_elem]);
                    }
                }
                else {
                    fprintf(dump_file, "Cant print all elems: Segmentation fault");
                    break;
                }
            }

            fprintf(dump_file, "\t}\n");
        }
        else {
            fprintf(dump_file, "\tdata[ERROR] \n");
        }
    }
#if DEBUG_MODE == DEBUG_MODE_ON
    else {
        fprintf(dump_file, 
            "Now variable: %s<%s> [0x%x] in File: %s   in Function: %s    in Line: %d\n",
            location_call.var_name, location_call.var_type, "ERROR", location_call.file_name, 
            location_call.func_name, location_call.num_line);
    }

    fprintf(dump_file, "\n---------------------------------\n");
#endif

    fclose(dump_file);
}