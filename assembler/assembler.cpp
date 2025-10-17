#include "assembler.h"

#define ERROR_SOURCE_ assembler
#define ERROR_SOURCE_TYPE_ assembler_t*
#define ERROR_TYPE_ assembler_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION
HANDLE_ERROR(ASSEMBLER_PARSER_ERROR)
HANDLE_ERROR(ASSEMBLER_WRITER_ERROR)
HANDLE_ERROR(ASSEMBLER_SYNTAX_ERROR)
HANDLE_ERROR(ASSEMBLER_ALLOCATION_ERROR)
END_PRINT_ERROR_FUNCTION

#include "../instruction.h"

#include "settings.h"

#include <assert.h>
#include <string.h>

void AssemblerInitialize(assembler_t *const assembler, char const *const input_filename, char const *const output_filename, bool silent) {
    assert(assembler);
    assert(input_filename);
    assert(output_filename);

    assembler->error = 0;
    assembler->labels = NULL;
    ParserInitialize(&assembler->parser, input_filename);
    ERROR_ASSERT(assembler->parser.error == 0, ASSEMBLER_PARSER_ERROR);

    WriterInitialize(&assembler->writer, output_filename, silent);
    ERROR_ASSERT(assembler->writer.error == 0, ASSEMBLER_WRITER_ERROR);
}

static label_t const *FindLabel(assembler_t *const assembler, token_t *const token) {
    assert(assembler);
    assert(token);

    char const *const name = token->data.label_data.name;
    size_t const length = token->data.label_data.length;

    for (label_t const *next_label = assembler->labels; next_label != NULL; next_label = next_label->next)
        if (length == next_label->length && (strncmp(name, next_label->name, length) == 0))
            return next_label;
    return NULL;
}

static int AppendLabel(assembler_t *const assembler, token_t *const token, size_t jump_destination) {
    assert(assembler);
    assert(token);
    
    char const *const name = token->data.label_data.name;
    size_t const length = token->data.label_data.length;

    label_t **next_label_field = &assembler->labels;
    while (*next_label_field != NULL) {
        if ((*next_label_field)->length == length && (strncmp((*next_label_field)->name, name, length) == 0))
            return 1;
        next_label_field = &(*next_label_field)->next;
    }
    
    *next_label_field = (label_t *)calloc(1, sizeof(**next_label_field));
    if (next_label_field == NULL) 
        return -1;

    (*next_label_field)->name = token->data.label_data.name;
    (*next_label_field)->length = token->data.label_data.length;
    (*next_label_field)->jump_destination = jump_destination;
    (*next_label_field)->next = NULL;
    return 0;
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
                // TODO PUSH selects PUSHR & PUSH
                ERROR_ASSERT(needed_argument_count == 0, ASSEMBLER_SYNTAX_ERROR);
                WriteElement(&assembler->writer, &token.data.instruction_data, sizeof(token.data.instruction_data));
                instruction = token.data.instruction_data;
                needed_argument_count = INSTRUCTIONS[token.data.instruction_data].argument_count;
                break;

            case REGISTER_TOKEN:
                ERROR_ASSERT(needed_argument_count == 1, ASSEMBLER_SYNTAX_ERROR);
                ERROR_ASSERT(instruction == PUSHR_code || instruction == POPR_code, ASSEMBLER_SYNTAX_ERROR);
                WriteElement(&assembler->writer, &token.data.register_data, sizeof(token.data.register_data));
                needed_argument_count--;
                break;

            case LABEL_TOKEN:
                // TODO check for commands
                if (needed_argument_count == 0) {
                    int const code = AppendLabel(assembler, &token, assembler->writer.global_index);
                    ERROR_ASSERT(code != -1, ASSEMBLER_ALLOCATION_ERROR);
                    ERROR_ASSERT(!assembler->writer.silent || code != 1, ASSEMBLER_SYNTAX_ERROR);
                } else {
                    label_t const *const label = FindLabel(assembler, &token);

                    instruction_pointer_t jump_destination = 0;

                    ERROR_ASSERT(assembler->writer.silent || label != NULL, ASSEMBLER_SYNTAX_ERROR);
                    if (label != NULL)
                        jump_destination = label->jump_destination;

                    WriteElement(&assembler->writer, &jump_destination, sizeof(jump_destination));
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
    assert(assembler);

    if (assembler->labels != NULL) {
        label_t *next_label = assembler->labels;
        while(next_label != NULL) {
            label_t *tmp = next_label;
            next_label = next_label->next;
            free(tmp);
        }
    }

    ParserFinalize(&assembler->parser);
    WriterFinalize(&assembler->writer);
}
