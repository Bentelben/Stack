#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "../byteio.h"
#include "text_utils.h"

enum translator_error_t {
    TRANSLATOR_NO_ERROR = 0,
    TRANSLATOR_TOO_LONG_TOKEN_ERROR,
    TRANSLATOR_INVALID_COMMAND_ERROR,
    TRANSLATOR_INVALID_ARGUMENT_ERROR,
    TRANSLATOR_TOO_MANY_ARGUMENTS_ERROR,
    TRANSLATOR_TOO_FEW_ARGUMENTS_ERROR
};

translator_error_t TranslateText(writer_t *writer, line_t *lines, size_t line_count, size_t *text_progress, size_t *line_progress);
translator_error_t TranslateLine(writer_t *writer, char **line);


#endif
