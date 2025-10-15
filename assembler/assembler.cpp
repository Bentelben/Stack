#include "assembler.h"

#define ERROR_SOURCE_ assembler
#define ERROR_SOURCE_TYPE_ assembler_t*
#define ERROR_TYPE_ assembler_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION
HANDLE_ERROR(ASSEMBLER_PARSER_ERROR)
HANDLE_ERROR(ASSEMBLER_WRITER_ERROR)
HANDLE_ERROR(ASSEMBLER_SYNTAX_ERROR)
END_PRINT_ERROR_FUNCTION

#include <assert.h>

#include "../instruction.h"

#include "settings.h"

void AssemblerInitialize(assembler_t *const assembler, char const *const input_filename, char const *const output_filename) {
    assert(assembler);
    assert(input_filename);
    assert(output_filename);

    assembler->error = 0;
    ParserInitialize(&assembler->parser, input_filename);
    ERROR_ASSERT(assembler->parser.error == 0, ASSEMBLER_PARSER_ERROR);

    WriterInitialize(&assembler->writer, output_filename);
    ERROR_ASSERT(assembler->writer.error == 0, ASSEMBLER_WRITER_ERROR);

    for (size_t i = 0; i < 256; i++) {
        assembler->labels[i] = -1;
    }
}

void Assemble(assembler_t *const assembler) {
    uint8_t instruction = 0;
    size_t needed_argument_count = 0;
    while (1) {
        token_t token = {};
        ParseToken(&assembler->parser, &token);
        ERROR_ASSERT(assembler->parser.error == 0, ASSEMBLER_PARSER_ERROR);
        if (assembler->parser.isEOF)
            break;

        switch(token.type) {
            case NUMBER_TOKEN:
                ERROR_ASSERT(needed_argument_count > 0, ASSEMBLER_SYNTAX_ERROR);
                WriteElement(&assembler->writer, &token.data.number_data, sizeof(token.data.number_data));
                needed_argument_count--;
                break;

            case INSTRUCTION_TOKEN:
                ERROR_ASSERT(needed_argument_count == 0, ASSEMBLER_SYNTAX_ERROR);
                WriteElement(&assembler->writer, &token.data.instruction_data, sizeof(token.data.instruction_data));
                needed_argument_count = INSTRUCTIONS[token.data.instruction_data].argument_count;
                break;

            case REGISTER_TOKEN:
                ERROR_ASSERT(needed_argument_count == 1, ASSEMBLER_SYNTAX_ERROR);
                ERROR_ASSERT(instruction == PUSH_code || instruction == PUSHR_code || instruction == POPR_code, ASSEMBLER_SYNTAX_ERROR);
                WriteElement(&assembler->writer, &token.data.register_data, sizeof(token.data.register_data));
                needed_argument_count--;
                break;

            case LABEL_TOKEN:
                ERROR_ASSERT(token.data.label_data.label_code < 256, ASSEMBLER_SYNTAX_ERROR);
                // TODO check for commands
                if (needed_argument_count == 0) {
                    //ERROR_ASSERT(assembler->labels[token.data.label_data.label_code] == -1, ASSEMBLER_SYNTAX_ERROR);
                    assembler->labels[token.data.label_data.label_code] = (int)assembler->writer.global_index;
                } else {
                    WriteElement(&assembler->writer, &assembler->labels[token.data.label_data.label_code], sizeof(assembler->labels[0]));
                    needed_argument_count--;
                }
                break;
            default:
                RAISE_ERROR(ASSEMBLER_SYNTAX_ERROR);
                break;
        }
    }
}

void AssemblerFinalize(assembler_t *const assembler) {
    ParserFinalize(&assembler->parser);
    WriterFinalize(&assembler->writer);
}
