#ifndef COMMANDS_H__
#define COMMANDS_H__

#define DEF_CMD(cmd, ...) CMD_##cmd,

enum class PROCESSOR_COMMANDS {
    CMD_ZERO,
#include "cmd_def.h"
    CMD_MAX
};

#undef DEF_CMD

#endif