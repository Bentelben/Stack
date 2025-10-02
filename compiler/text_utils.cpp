#include "text_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <sys/stat.h>

char *ReadFile(char const *const filename) {
    size_t text_size = 0;


    assert(filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    struct stat file_stat = {};
    if (stat(filename, &file_stat) == -1)
        return NULL;

    size_t file_size = (size_t)file_stat.st_size;

    char *text = (char *)calloc(file_size + 1, sizeof(*text));
    if (text == NULL)
        return NULL;

    text_size = fread(text, sizeof(*text), file_size, file);
    text[text_size] = '\0';

    fclose(file);
    return text;
}

line_t *GetLineArray(char *const text, size_t *const line_count) {
    assert(text);

    *line_count = 0;
    char *iterator = text;
    while (1) {
        *line_count += 1;
        iterator = strchr(iterator + 1, '\n');
        if (iterator == NULL)
            break;
    }

    line_t *lines = (line_t *)calloc(*line_count, sizeof(*lines));
    if (lines == NULL)
        return NULL;

    lines[0].str = text;
    for (size_t i = 1; i < *line_count; i++) {
        lines[i].str = strchr(lines[i-1].str, '\n') + 1;
        lines[i-1].length = (size_t)(lines[i].str - lines[i-1].str - 1);
    }
    lines[*line_count-1].length = strlen(lines[*line_count-1].str);
    
    if (lines[*line_count - 1].length == 0)
        (*line_count)--;

    return lines;
}

void FPrintLine(FILE *const file, line_t const line) {
    fprintf(file, "`%.*s`\n", (int)line.length, line.str);
}

void PrintLine(line_t const line) {
    FPrintLine(stdout, line);
}

void FPrintLineArray(FILE *const file, line_t const *const lines, size_t const line_count) {
    for (size_t i = 0; i < line_count; i++)
        FPrintLine(file, lines[i]);
}

void PrintLineArray(line_t const *const lines, size_t const line_count) {
    FPrintLineArray(stdout, lines, line_count);
}
