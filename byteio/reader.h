#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum reader_error_offset_t {
    READER_ALLOCATION_ERROR,
    READER_FILE_READING_ERROR,
    READER_EOF_ERROR
};

typedef uint8_t reader_error_t;

struct reader_t {
    reader_error_t error;
    char *array;
    size_t size;
    size_t index;
};

int ReaderInitialize(reader_t *reader, char const *filename);
void SetReaderPosition(reader_t *reader, size_t position);
bool CanRead(reader_t *reader);
int ReadElement(reader_t *reader, void *pointer, size_t size);
void ReaderFinalize(reader_t *reader);

#endif
