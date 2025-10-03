#ifndef BYTEIO_H
#define BYTEIO_H

#include <stdio.h>
#include <stdlib.h>

struct reader_t {
    char *array;
    size_t size;
    size_t index;
};

int InitializeReader(reader_t *reader, char const *filename);
bool CanRead(reader_t *reader);
int ReadElement(reader_t *reader, void *pointer, size_t size);
void FinalizeReader(reader_t *reader);

const size_t WRITER_BUFFER_SIZE = 4096;

struct writer_t {
    FILE *file;
    char *array;
    size_t index;
};

int InitializeWriter(writer_t *writer, char const *filename);
int WriterFlush(writer_t *writer);
int WriteElement(writer_t *writer, void *pointer, size_t size);
void FinalizeWriter(writer_t *writer);


#endif
