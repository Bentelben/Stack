#include "assembler.h"

#define ERROR_SOURCE_ assembler
#define ERROR_SOURCE_TYPE_ assembler_t*
#define ERROR_TYPE_ assembler_error_t

#include "../error_handler.h"

START_PRINT_ERROR_FUNCTION
HANDLE_ERROR(ASSEMBLER_PARSER_ERROR)
HANDLE_ERROR(ASSEMBLER_WRITER_ERROR)
HANDLE_ERROR(ASSEMBLER_ALLOCATION_ERROR)
HANDLE_ERROR(ASSEMBLER_EXPECTED_INSTRUCTION_ERROR)
HANDLE_ERROR(ASSEMBLER_EXPECTED_LABEL_OR_ADDRESS_ERROR)
HANDLE_ERROR(ASSEMBLER_EXPECTED_NUMBER_ERROR)
HANDLE_ERROR(ASSEMBLER_EXPECTED_REGISTER_ERROR)
HANDLE_ERROR(ASSEMBLER_UNKNOWN_LABEL_ERROR)
END_PRINT_ERROR_FUNCTION

#include "../instruction.h"

#include "settings.h"

#include <assert.h>
#include <string.h>

static void AssembleData(assembler_t *assembler, void *data, size_t size, assembler_stage_t stage) {
    assembler->byteArrayIndex += size;
    if (stage == ASSEMBLER_MAIN_STAGE)
        WriterWriteElement(&assembler->writer, data, size);
}

static void ParseTokens(assembler_t *assembler) {
    assert(assembler);

    ParserInitialize(&assembler->parser, assembler->inputFileName);
    ERROR_ASSERT(assembler->parser.error == 0, ASSEMBLER_PARSER_ERROR);

    while (1) {
        if (assembler->nTokens >= assembler->tokenCapacity) {
            if (assembler->tokenCapacity == 0)
                assembler->tokenCapacity = 8;

            assembler->tokens = (token_t *)realloc(assembler->tokens, sizeof(*assembler->tokens)*assembler->tokenCapacity*2);
            assert(assembler->tokens);
            assembler->tokenCapacity *= 2;
        }

        ParseToken(&assembler->parser, assembler->tokens + assembler->nTokens);
        ERROR_ASSERT(assembler->parser.error == 0, ASSEMBLER_PARSER_ERROR);
        if (assembler->parser.isEOF)
            break;

        assembler->nTokens++;
    }
}

void AssemblerInitialize(assembler_t *const assembler, char const *const inputFileName, char const *const outputFileName) {
    assert(assembler);
    assert(inputFileName);
    assert(outputFileName);

    assembler->error = 0;
    assembler->inputFileName = inputFileName;
    assembler->outputFileName = outputFileName;

    assembler->tokens = NULL;
    assembler->nTokens = 0;
    assembler->tokenCapacity = 0;
    assembler->tokenIndex = 0;
    assembler->byteArrayIndex = 0;

    assembler->labels = NULL;
    assembler->nLabels = 0;
    assembler->labelCapacity = 0;

    WriterInitialize(&assembler->writer, outputFileName);
    ERROR_ASSERT(assembler->writer.error == 0, ASSEMBLER_WRITER_ERROR);

    ParseTokens(assembler);
    if (assembler->error != 0)
        return;
}

void AssemblerRun(assembler_t *assembler) {
    assert(assembler);

    AssemblerRunStage(assembler, ASSEMBLER_LABEL_LINKING_STAGE);
    if (assembler->error != 0)
        return;
    AssemblerRunStage(assembler, ASSEMBLER_MAIN_STAGE);
}

static label_t *FindLabel(assembler_t *const assembler, token_t *const token) {
    assert(assembler);
    assert(token);

    char const *const name = token->data.label_data.name;
    size_t const length = token->data.label_data.length;

    for (size_t i = 0; i < assembler->nLabels; i++)
        if (length == assembler->labels[i].length && (strncmp(name, assembler->labels[i].name, length) == 0))
            return assembler->labels + i;
    
    return NULL;
}

static void AppendLabel(assembler_t *const assembler, token_t *const token, size_t jump_destination) {
    assert(assembler);
    assert(token);
    
    if (assembler->nLabels >= assembler->labelCapacity) {
        if (assembler->labelCapacity == 0)
            assembler->labelCapacity = 8;
        assembler->labelCapacity *= 2;
        assembler->labels = (label_t *)realloc(assembler->labels, assembler->labelCapacity*sizeof(*assembler->labels));
        assert(assembler->labels);
    }

    label_t *label = assembler->labels + assembler->nLabels++;
    label->name = token->data.label_data.name;
    label->length = token->data.label_data.length;
    label->jump_destination = jump_destination;
}

static void AssembleRegister(assembler_t *const assembler, assembler_stage_t stage) {
    token_t *token = assembler->tokens + assembler->tokenIndex;
    ERROR_ASSERT(token->type == REGISTER_TOKEN, ASSEMBLER_EXPECTED_REGISTER_ERROR);

    AssembleData(assembler, &token->data.register_data, sizeof(token->data.register_data), stage);
}

static void AssembleNumber(assembler_t *const assembler, assembler_stage_t stage) {
    token_t *token = assembler->tokens + assembler->tokenIndex;
    ERROR_ASSERT(token->type == NUMBER_TOKEN, ASSEMBLER_EXPECTED_NUMBER_ERROR);

    AssembleData(assembler, &token->data.number_data, sizeof(token->data.number_data), stage);
}

static void AssembleLabelOrAddress(assembler_t *const assembler, assembler_stage_t stage) {
    token_t *token = assembler->tokens + assembler->tokenIndex;
    ERROR_ASSERT(token->type == NUMBER_TOKEN || token->type == LABEL_TOKEN, ASSEMBLER_EXPECTED_LABEL_OR_ADDRESS_ERROR);
    if (token->type == NUMBER_TOKEN) {
        AssembleData(assembler, &token->data.number_data, sizeof(token->data.number_data), stage);
    } else {
        size_t jumpDestination = 0;
        if (stage == ASSEMBLER_MAIN_STAGE) {
            label_t *label = FindLabel(assembler, token);
            ERROR_ASSERT(label, ASSEMBLER_UNKNOWN_LABEL_ERROR);
            jumpDestination = label->jump_destination;
        }
        AssembleData(assembler, &jumpDestination, sizeof(jumpDestination), stage);
    }
}

static void AssembleInstruction(assembler_t *const assembler, assembler_stage_t stage) {
    token_t *instructionToken = assembler->tokens + assembler->tokenIndex;
    ERROR_ASSERT(instructionToken->type == INSTRUCTION_TOKEN, ASSEMBLER_EXPECTED_INSTRUCTION_ERROR);

    instruction_code_t instruction = instructionToken->data.instruction_data;

    ERROR_ASSERT(assembler->nTokens - assembler->tokenIndex - 1 >= INSTRUCTIONS[instruction].argument_count, ASSEMBLER_TOO_FEW_ARGUMENTS_ERROR);

    if (instruction == PUSH_code) {
        if (instructionToken[1].type == REGISTER_TOKEN)
            instruction = PUSHR_code;
    }

    AssembleData(assembler, &instruction, sizeof(instruction), stage);
    assembler->tokenIndex++;

    if (INSTRUCTIONS[instruction].argument_count == 0)
        return;
    
    switch(instruction) {
        case PUSH_code:
            AssembleNumber(assembler, stage);
            CHECK_RETURN;
            assembler->tokenIndex++;
            break;
        case POPR_code:
        case POPM_code:
        case POPV_code:
            AssembleRegister(assembler, stage);
            CHECK_RETURN;
            assembler->tokenIndex++;
            break;
        case PUSHR_code:
        case PUSHM_code:
        case PUSHV_code:
            AssembleRegister(assembler, stage);
            CHECK_RETURN;
            assembler->tokenIndex++;
            break;
        case JMP_code:
        case JB_code:
        case JBE_code:
        case JA_code:
        case JAE_code:
        case JE_code:
        case JNE_code:
            AssembleLabelOrAddress(assembler, stage);
            CHECK_RETURN;
            assembler->tokenIndex++;
            break;
        case CALL_code:
            AssembleLabelOrAddress(assembler, stage);
            CHECK_RETURN;
            assembler->tokenIndex++;
            break;
        default:
            assert(0);
            break;
    }
}

void AssemblerRunStage(assembler_t *const assembler, assembler_stage_t stage) {
    assembler->tokenIndex = 0;
    assembler->byteArrayIndex = 0;
    while (assembler->tokenIndex < assembler->nTokens) {
        token_t *token = assembler->tokens + assembler->tokenIndex;
        if (token->type == LABEL_TOKEN) {
            if (stage == ASSEMBLER_LABEL_LINKING_STAGE) {
                ERROR_ASSERT(FindLabel(assembler, token) == NULL, ASSEMBLER_LABEL_REDEFINITION_ERROR);
                AppendLabel(assembler, token, assembler->byteArrayIndex);
            }
            
            assembler->tokenIndex++;
        } else {
            AssembleInstruction(assembler, stage);
            CHECK_RETURN;
        }
    }
}

void AssemblerFinalize(assembler_t *const assembler) {
    assert(assembler);

    free(assembler->tokens);
    free(assembler->labels);

    WriterFinalize(&assembler->writer);
    ParserFinalize(&assembler->parser);
}
