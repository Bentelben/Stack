#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <stdio.h>

typedef struct _line_t {
    char *str;
    size_t length;
} line_t;

char *ReadFile(char const *file);
line_t *GetLineArray(char *text, size_t *line_count);

void FPrintLine(FILE *file, line_t line);
void PrintLine(line_t line);

void FPrintLineArray(FILE *file, const line_t *lines, size_t line_count);
void PrintLineArray(const line_t *lines, size_t line_count);

#endif
