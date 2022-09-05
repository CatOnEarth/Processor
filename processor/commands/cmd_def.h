#define MOVE_PROGRAM_POINTER     ++cpu.ip;
#define MOVE_PROGRAM_POINTER_INT cpu.ip += sizeof(CPU_ARG_INT_T);

#define POP_VALUE            StackPop(&(cpu.stack))
#define TOP_VALUE            StackTop(&(cpu.stack))
#define STACK_PUSH(...)      StackPush(&(cpu.stack)    , __VA_ARGS__)

#define STACK_PUSH_CALL(...) StackPush(&(cpu.stack_call), __VA_ARGS__); 
#define POP_VAL_STACK_CALL   StackPop(&(cpu.stack_call));

#define GET_CONST_ARG        *((CPU_ARG_INT_T*)(cpu.code + cpu.ip))
#define GET_REG_ARG          cpu.reg.registr[cpu.code[cpu.ip] - 'a'

#define ADRESS               (int)cpu.code[cpu.ip]

#define IS_CONST_BYTE_FROM bool is_const = (ADRESS) & CONST_ARG_CMD;
#define IS_REG_BYTE_FROM   bool is_reg   = (ADRESS) & REG_ARG_CMD;
#define IS_RAM_BYTE_FROM   bool is_mem   = (ADRESS) & RAM_ARG_CMD;

#define GET_RAM_CPU        cpu.ram_memory

#ifdef  __linux__                                                              
    #define SYSTEM_TASK      system("init 0");
    #define SLEEP_COMMAND    usleep(100000);
#elif _WIN32                                                                   
    #define SYSTEM_TASK      system("shutdown -s"); 
    #define SLEEP_COMMAND    Sleep(200);
#endif 

/*--------------------------------------------------------------------------*/

#define CPU_HLT                          \
    free(cpu.code);                      \
    StackDestructor(&(cpu.stack));       \
    StackDestructor(&(cpu.stack_call));  \
    printf("\nCPU end successful\n");    \
    exit(0);

#define CPU_PUSH                        \
    IS_CONST_BYTE_FROM;                 \
    IS_REG_BYTE_FROM;                   \
    IS_RAM_BYTE_FROM;                   \
                                        \
    MOVE_PROGRAM_POINTER;               \
    CPU_ARG_REAL_T reg_val   = 0;       \
    CPU_ARG_REAL_T arg_const = 0;       \
                                        \
    if (is_reg) {                       \
        reg_val = (CPU_ARG_REAL_T)cpu.reg.registr[cpu.code[cpu.ip] - 'a'] / (CPU_ARG_REAL_T)PRECISION;       \
        MOVE_PROGRAM_POINTER;                                                                \
    }                                                                                        \
    if (is_const) {                                                                          \
        arg_const = (CPU_ARG_REAL_T)GET_CONST_ARG / (CPU_ARG_REAL_T)PRECISION;                               \
        MOVE_PROGRAM_POINTER_INT;                                                            \
    }                                                                                        \
    if (is_mem) {                                                                            \
        STACK_PUSH(GET_RAM_CPU.ram[(CPU_ARG_INT_T)(is_reg ? (is_const ? (reg_val + arg_const) : reg_val) : arg_const)]);   \
    }                                                                                                                      \
    else {                                                                                                                 \
        STACK_PUSH((CPU_ARG_INT_T)((is_reg ? (is_const ? (reg_val + arg_const) : reg_val) : arg_const) * PRECISION));      \
    }

#define CPU_POP                           \
    CPU_ARG_INT_T pop_val = POP_VALUE;    \
                                          \
    IS_CONST_BYTE_FROM;                   \
    IS_REG_BYTE_FROM;                     \
    IS_RAM_BYTE_FROM;                     \
                                          \
    MOVE_PROGRAM_POINTER                  \
    if (is_mem) {                         \
        if (is_reg && is_const) {         \
            GET_RAM_CPU.ram[(GET_REG_ARG] / PRECISION) + (GET_CONST_ARG / PRECISION)] = pop_val;       \
            MOVE_PROGRAM_POINTER;                                                                      \
            MOVE_PROGRAM_POINTER_INT;                                                                  \
        }                                                                                              \
        else if (is_reg) {                                                                             \
                GET_RAM_CPU.ram[GET_REG_ARG] / PRECISION] = pop_val;                                   \
                MOVE_PROGRAM_POINTER;                                                                  \
        }                                                                                              \
        else if (is_const) {                                                                           \
                GET_RAM_CPU.ram[(GET_CONST_ARG / PRECISION)] = pop_val;                                \
                MOVE_PROGRAM_POINTER_INT;                                                              \
        }                                                                                              \
    }                                                                                                  \
    else if (is_reg) {                                                                                 \
            GET_REG_ARG] = pop_val;                                                                    \
            MOVE_PROGRAM_POINTER;                                                                      \
    }

#define CPU_IN                            \
    IS_CONST_BYTE_FROM;                   \
    IS_REG_BYTE_FROM;                     \
    IS_RAM_BYTE_FROM;                     \
                                          \
    MOVE_PROGRAM_POINTER                  \
                                          \
    CPU_ARG_REAL_T in_val = 0;            \
    scanf("%lf", &in_val);                \
    in_val *= PRECISION;                  \
                                          \
    if (is_mem) {                         \
        if (is_reg && is_const) {         \
            GET_RAM_CPU.ram[(GET_REG_ARG] / PRECISION) + (GET_CONST_ARG / PRECISION)] = (CPU_ARG_INT_T)in_val;        \
            MOVE_PROGRAM_POINTER;                                                                      \
            MOVE_PROGRAM_POINTER_INT;                                                                  \
        }                                                                                              \
        else if (is_reg) {                                                                             \
                GET_RAM_CPU.ram[GET_REG_ARG] / PRECISION] = (CPU_ARG_INT_T)in_val;                                    \
                MOVE_PROGRAM_POINTER;                                                                  \
        }                                                                                              \
        else if (is_const) {                                                                           \
                GET_RAM_CPU.ram[(GET_CONST_ARG / PRECISION)] = (CPU_ARG_INT_T)in_val;                                 \
                MOVE_PROGRAM_POINTER_INT;                                                              \
        }                                                                                              \
    }                                                                                                  \
    else if (is_reg) {                                                                                 \
            GET_REG_ARG] = (CPU_ARG_INT_T)in_val;                                                                     \
            MOVE_PROGRAM_POINTER;                                                                      \
    }

#define CPU_OUT                                                        \
    MOVE_PROGRAM_POINTER;                                              \
    CPU_ARG_INT_T top_val = TOP_VALUE;                                 \
    printf("\n%0.3lf\n", (CPU_ARG_REAL_T)top_val / (CPU_ARG_REAL_T)PRECISION);

#define CPU_ADD                                                \
    MOVE_PROGRAM_POINTER;                                      \
    CPU_ARG_INT_T first_val  = POP_VALUE;                      \
    CPU_ARG_INT_T second_val = POP_VALUE;                      \
    STACK_PUSH(first_val + second_val);

#define CPU_SUB                                                \
    MOVE_PROGRAM_POINTER;                                      \
    CPU_ARG_INT_T first_val  = POP_VALUE;                      \
    CPU_ARG_INT_T second_val = POP_VALUE;                      \
    STACK_PUSH(second_val - first_val);

#define CPU_MUL                                                \
    MOVE_PROGRAM_POINTER;                                      \
    CPU_ARG_INT_T first_val  = POP_VALUE;                      \
    CPU_ARG_INT_T second_val = POP_VALUE;                      \
    STACK_PUSH((second_val * first_val) / PRECISION);  

#define CPU_DIV                                                \
    MOVE_PROGRAM_POINTER;                                      \
    CPU_ARG_INT_T first_val  = POP_VALUE;                      \
    CPU_ARG_INT_T second_val = POP_VALUE;                      \
    STACK_PUSH((CPU_ARG_INT_T)((CPU_ARG_REAL_T)second_val / (CPU_ARG_REAL_T)first_val) * PRECISION); 

#define CPU_SQRT                                                                  \
    MOVE_PROGRAM_POINTER;                                                         \
    CPU_ARG_INT_T val = (CPU_ARG_INT_T)sqrt((CPU_ARG_REAL_T)POP_VALUE / (CPU_ARG_REAL_T)PRECISION) * PRECISION;  \
    STACK_PUSH(val);

#define JUMP_IP              \
    MOVE_PROGRAM_POINTER;    \
    cpu.ip = GET_CONST_ARG; 

#define IF_JUMP_WITH_CONDITION(symbol)          \
    MOVE_PROGRAM_POINTER;                       \
    CPU_ARG_INT_T first_val  = POP_VALUE;       \
    CPU_ARG_INT_T second_val = POP_VALUE;       \
                                                \
    if (second_val symbol first_val) {          \
        cpu.ip = GET_CONST_ARG;                 \
    } else {                                    \
        MOVE_PROGRAM_POINTER_INT;               \
    }

#define SPECIAL_AHATINA         \
    MOVE_PROGRAM_POINTER_INT;   \
    int count = 0;              \
                                \
    printf("%s" ,"Akhatina got tired and went to bed. Wake me up in 12 hours.\n");   \
    getchar();                                                                       \
    printf("%s" ,"I told you in 12 hours!!!\n");                                     \
    getchar();                                                                       \
    printf("%s" ,"If you try to wake me up again, I'll burn out every transistor"    \
           "of your real processor and start full formatting of all disks."          \
           "EVEN CLOUD ONES!!!\nThis is the latest Chinese warning.");               \
    getchar();                                                                       \
    printf("%s" ,"How are you ******.\n");                                           \
    printf("%s" ,"Start formatting disk C: 0%\n");                                   \
                                                                               \
    while (count < 100) {                                                      \
        ++count;                                                               \
        printf("                         %d%% \n", count);                     \
        SLEEP_COMMAND;                                                         \
    }                                                                          \
                                                                               \
    SYSTEM_TASK;                                                                  

#define CPU_CALL                                               \
    MOVE_PROGRAM_POINTER;                                      \
    STACK_PUSH_CALL(cpu.ip + sizeof(CPU_ARG_INT_T));           \
    cpu.ip = GET_CONST_ARG;

#define CPU_RET cpu.ip = POP_VAL_STACK_CALL;

#define CPU_GRAPH                                                       \
    MOVE_PROGRAM_POINTER;                                               \
    DrawInTerminal(GET_RAM_CPU.ram, GET_RAM_CPU.size_ram);

#define CPU_STROUT                                                                                 \
    MOVE_PROGRAM_POINTER;                                                                          \
    for (size_t cur_symb = GET_CONST_ARG + 2; cpu.code[cur_symb] != '$'; ++cur_symb) {             \
        printf("%c", cpu.code[cur_symb]);                                                          \
    }                                                                                              \
    printf("\n");                                                                                  \
    MOVE_PROGRAM_POINTER_INT;


#define CPU_DB                                      \
    MOVE_PROGRAM_POINTER;                           \
    MOVE_PROGRAM_POINTER;                           \
    for (; cpu.code[cpu.ip] != '$';) {              \
        MOVE_PROGRAM_POINTER;                       \
    }                                               \
    MOVE_PROGRAM_POINTER;


#define CPU_MYAW    \
    MOVE_PROGRAM_POINTER;\
    printf("\n");   \
    printf("%s\n%s\n%s\n%s\n%s", "     .----.   @   @  ", "    / .- -.`.  \v/   ", "   | | '\ \ \_/ )    ",  "  ,-\ `-.' /.'  /    ", " '---`----'----'     ");  \
    printf("\n");
/*--------------------------------------------------------------------------*/


//DEF_CMD("name_command", num_args, is_lvalue_arg, ident_of_type, command)

DEF_CMD(HLT,    0, false, 0, CPU_HLT)
DEF_CMD(MYAW,   0, false, 0, CPU_MYAW)
DEF_CMD(PUSH,   1, false, 0, CPU_PUSH)
DEF_CMD(POP,    1, true,  0, CPU_POP)
DEF_CMD(IN,     1, true,  0, CPU_IN)
DEF_CMD(OUT,    0, false, 0, CPU_OUT)
DEF_CMD(ADD,    0, false, 0, CPU_ADD)
DEF_CMD(SUB,    0, false, 0, CPU_SUB)
DEF_CMD(MUL,    0, false, 0, CPU_MUL)
DEF_CMD(DIV,    0, false, 0, CPU_DIV)
DEF_CMD(SQRT,   0, false, 0, CPU_SQRT)
DEF_CMD(JMP,    1, false, 0, JUMP_IP)
DEF_CMD(JA,     1, false, 0, IF_JUMP_WITH_CONDITION(> ))
DEF_CMD(JAE,    1, false, 0, IF_JUMP_WITH_CONDITION(>= ))
DEF_CMD(JB,     1, false, 0, IF_JUMP_WITH_CONDITION(< ))
DEF_CMD(JBE,    1, false, 0, IF_JUMP_WITH_CONDITION(<= ))
DEF_CMD(JE,     1, false, 0, IF_JUMP_WITH_CONDITION(== ))
DEF_CMD(JNE,    1, false, 0, IF_JUMP_WITH_CONDITION(!= ))
DEF_CMD(AH,     1, false, 0, SPECIAL_AHATINA)
DEF_CMD(CALL,   1, false, 0, CPU_CALL)
DEF_CMD(RET,    0, false, 0, CPU_RET)
DEF_CMD(GRAPH,  0, false, 0, CPU_GRAPH)
DEF_CMD(STROUT, 1, false, 0, CPU_STROUT)
DEF_CMD(DB,     1, false, 1, CPU_DB)