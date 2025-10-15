#include "reader.h"


#define ERROR_SOURCE_ reader
#define ERROR_SOURCE_TYPE_ reader_t*
#define ERROR_TYPE_ reader_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION()
HANDLE_ERROR(READER_ALLOCATION_ERROR)
HANDLE_ERROR(READER_FILE_READING_ERROR)
HANDLE_ERROR(READER_TOO_LONG_TOKEN_ERROR)
HANDLE_ERROR(READER_EOF_ERROR)
END_PRINT_ERROR_FUNCTION()

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>

static size_t GetFileSize(char const *const filename) {
    struct stat file_stat = {};
    if (stat(filename, &file_stat) == -1)
        return 0;
    return (size_t)file_stat.st_size;
}

void ReaderInitialize(reader_t *reader, char const *const filename) {
    assert(reader);

    size_t file_size = GetFileSize(filename);
    ERROR_ASSERT(file_size != 0, READER_FILE_READING_ERROR);

    FILE *file = fopen(filename, "rb");
    ERROR_ASSERT(file != NULL, READER_FILE_READING_ERROR);

    reader->array = (char *)calloc(file_size, sizeof(*reader->array));
    if (reader->array == NULL) {
        fclose(file);
        RAISE_ERROR(READER_ALLOCATION_ERROR);
    }

    size_t bytes_read = fread(reader->array, sizeof(*reader->array), file_size, file);
    if (bytes_read != file_size) {
        fclose(file);
        free(reader->array);
        RAISE_ERROR(READER_FILE_READING_ERROR);
    }

    fclose(file);

    reader->size = file_size;
    reader->index = 0;
}

void SetReaderPosition(reader_t *reader, size_t position) {
    reader->index = position;
}

bool CanRead(reader_t *const reader) {
    return reader->index < reader->size;
}

void ReadElement(reader_t *reader, void *const pointer, size_t const size) {
    assert(reader);

    ERROR_ASSERT(reader->index + size - 1 < reader->size, READER_TOO_LONG_TOKEN_ERROR);

    memcpy(pointer, reader->array + reader->index, size);
    reader->index += size;
}

void ReaderFinalize(reader_t *const reader) {
    assert(reader);

    free(reader->array);
}


