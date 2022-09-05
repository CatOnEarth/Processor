#pragma once

/*--------------------------------------------------------------------------*/

#include <cstdio>

/*--------------------------------------------------------------------------*/

#include "../../types/types.h"
#ifdef __linux__
    #include <SFML/Graphics.hpp>
#endif

/*--------------------------------------------------------------------------*/

const int HEIGHT_WINDOW = 300;
const int WIDTH_WINDOW  = 300;

/*--------------------------------------------------------------------------*/

#ifdef __linux__
    void DrawInTerminal(CPU_ARG_INT_T* memory, size_t start_ind_video_mem);
#elif _WIN32
    void DrawInTerminal(CPU_ARG_INT_T* memory, size_t start_ind_video_mem);
#endif

