#include "byteio.h"

#include <assert.h>
#include <sys/stat.h>
#include <string.h>

static size_t GetFileSize(char const *filename) {
    struct stat file_stat = {};
    if (stat(filename, &file_stat) == -1)
        return 0;
    return (size_t)file_stat.st_size;
}

int InitializeReader(reader_t *reader, char const *filename) {
    assert(reader);

    size_t file_size = GetFileSize(filename);
    if (file_size == 0)
        return -1;

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return -1;

    reader->array = (char *)calloc(file_size, sizeof(*reader->array));
    if (reader->array == NULL) {
        fclose(file);
        return -1;
    }

    // ??
    size_t bytes_read = fread(reader->array, sizeof(*reader->array), file_size, file);
    if (bytes_read != file_size) {
        fclose(file);
        free(reader->array);
        return -1;
    }

    fclose(file);

    reader->size = file_size;
    reader->index = 0;
    return 0;
}

bool CanRead(reader_t *reader) {
    return reader->index < reader->size;
}

int ReadElement(reader_t *reader, void *pointer, size_t size) {
    assert(reader);

    if (reader->index + size - 1 >= reader->size)
        return -1;

    memcpy(pointer, reader->array + reader->index, size);
    reader->index += size;

    return 0;
}

void FinalizeReader(reader_t *reader) {
    assert(reader);

    free(reader->array);
}


int InitializeWriter(writer_t *writer, char const *filename) {
    assert(writer);

    writer->file = fopen(filename, "wb");
    if (writer->file == NULL)
        return -1;

    writer->array = (char *)calloc(WRITER_BUFFER_SIZE, sizeof(*writer->array));
    if (writer->array == NULL) {
        fclose(writer->file);
        return -1;
    }

    writer->index = 0;
    return 0;
}

int WriterFlush(writer_t *writer) {
    assert(writer);

    for (size_t i = 0; i < writer->index; i++) {
        printf("%d ", writer->array[i]);
    }
    printf("\n");

    if (writer->index == 0)
        return 0;
    size_t bytes_written = fwrite(writer->array, sizeof(*writer->array), writer->index, writer->file);
    if (bytes_written != writer->index)
        return -1;

    writer->index = 0;
    return 0;
}

int WriteElement(writer_t *writer, void *pointer, size_t size) {
    assert(writer);

    if (size > WRITER_BUFFER_SIZE)
        return -1;

    if (writer->index + size - 1 >= WRITER_BUFFER_SIZE)
        WriterFlush(writer);

    memcpy(writer->array + writer->index, pointer, size);
    writer->index += size;

    return 0;
}

void FinalizeWriter(writer_t *writer) {
    assert(writer);

    WriterFlush(writer);
    fclose(writer->file);
    free(writer->array);
}
