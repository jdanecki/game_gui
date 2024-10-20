#include "text.h"
#include <stdio.h>
#include <stdarg.h>

void print_status(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(status_line, format, args);
    va_end(args);
};
