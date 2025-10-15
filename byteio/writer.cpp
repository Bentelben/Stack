#include "writer.h"

#define ERROR_SOURCE_ writer
#define ERROR_SOURCE_TYPE_ writer_t*
#define ERROR_TYPE_ writer_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION
HANDLE_ERROR(WRITER_ALLOCATION_ERROR)
HANDLE_ERROR(WRITER_FILE_WRITING_ERROR)
END_PRINT_ERROR_FUNCTION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

void WriterInitialize(writer_t *const writer, char const *const filename) {
    assert(writer);

    writer->file = fopen(filename, "wb");
    ERROR_ASSERT(writer->file != NULL, WRITER_FILE_WRITING_ERROR);

    writer->array = (char *)calloc(WRITER_BUFFER_SIZE, sizeof(*writer->array));
    if (writer->array == NULL) {
        fclose(writer->file);
        RAISE_ERROR(WRITER_ALLOCATION_ERROR);
    }

    writer->index = 0;
    writer->global_index = 0;
}

void WriterFlush(writer_t *const writer) {
    assert(writer);

    if (writer->index == 0)
        return;

    size_t bytes_written = fwrite(writer->array, sizeof(*writer->array), writer->index, writer->file);
    if (bytes_written != writer->index)
        RAISE_ERROR(WRITER_FILE_WRITING_ERROR);

    writer->index = 0;
}

void WriteElement(writer_t *const writer, void *const pointer, size_t const size) {
    assert(writer);

    ERROR_ASSERT(size <= WRITER_BUFFER_SIZE, WRITER_ELEMENT_BIGGER_THAN_BUFFER_ERROR);

    if (writer->index + size - 1 >= WRITER_BUFFER_SIZE)
        WriterFlush(writer);

    memcpy(writer->array + writer->index, pointer, size);
    writer->index += size;
    writer->global_index += size;
}

void WriterFinalize(writer_t *const writer) {
    assert(writer);

    WriterFlush(writer);
    fclose(writer->file);
    free(writer->array);
}
