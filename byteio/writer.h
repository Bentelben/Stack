#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const size_t WRITER_BUFFER_SIZE = 4096;

enum writer_error_offset_t {
    WRITER_ALLOCATION_ERROR,
    WRITER_FILE_WRITING_ERROR,
    WRITER_ELEMENT_BIGGER_THAN_BUFFER_ERROR
};

typedef uint8_t writer_error_t;

struct writer_t {
    writer_error_t error;
    FILE *file;
    char *array;
    size_t index;
    size_t global_index;
    bool silent;
};

void WriterInitialize(writer_t *writer, char const *filename, bool silent = false);
void WriterFlush(writer_t *writer);
void WriteElement(writer_t *writer, void const *pointer, size_t size);
void WriterFinalize(writer_t *writer);



#endif
