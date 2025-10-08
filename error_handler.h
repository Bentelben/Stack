// Before including this header you MUST define:
// `ERROR_SOURCE_` - name of variable containing structor with error field
// `ERROR_SOURCE_TYPE_` - type of this field
// `ERROR_TYPE_` - type of error field
//
// Also you MUST use define
// `START_PRINT_ERROR_FUNCTION`
// `HANDLE_ERROR(error_enum_value, error_description)`
// `END_PRINT_ERROR_FUNCTION`

#define STR_(x) #x
#define STR(x) STR_(x)

#include <stdio.h>

#define ERROR_BIT_ ((ERROR_TYPE_)1)

// TODO remove skobochka
#define START_PRINT_ERROR_FUNCTION() \
static void FPrintError(FILE *const file, const ERROR_SOURCE_TYPE_ ERROR_SOURCE_) { \
    if (ERROR_SOURCE_ == NULL) \
        fprintf(stderr, STR(ERROR_SOURCE_) "_IS_NULL_ERROR\n"); \
        return; \
    if (ERROR_SOURCE_->error == 0) { \
        fprintf(file, "NO_ERROR\n"); \
        return; \
    } \
    ERROR_TYPE_ error = ERROR_SOURCE_->error; \
    size_t i = sizeof(error)*8 - 1; \
    for (; error > 0; i--) { \
        if (error >= (ERROR_BIT_ << i)) { \
            error -= (ERROR_TYPE_)(ERROR_BIT_ << i); \
            switch (i) {

#define HANDLE_ERROR(error_code) \
                case error_code: \
                    fprintf(file, #error_code "\n"); \
                    break;

#define END_PRINT_ERROR_FUNCTION() \
                default: \
                    fprintf(file, "%zu", i); \
                    break; \
            } \
            if (error != 0) \
                fprintf(file, ", "); \
        } \
        if (i == 0) \
            break; \
    } \
    fprintf(file, "\n"); \
}


// replace defines with function

#define LOG_ERROR() \
{ \
    fprintf(stderr, "Error occured in function %s in %s:%d\n", __func__, __FILE__, __LINE__); \
    FPrintError(stderr, ERROR_SOURCE_); \
}

#define SET_ERROR(error_code) \
    ERROR_SOURCE_->error |= ERROR_BIT_ << (error_code)

#define RESET_ERROR(error_code) \
    ERROR_SOURCE_->error &= (ERROR_TYPE_)(~(ERROR_BIT_ << (error_code)))

#define UPDATE_ERROR_VALUE(error_code, statement)   \
{                                               \
    if (statement) SET_ERROR(error_code); \
    else RESET_ERROR(error_code);         \
} 

#define RAISE_ERROR(error_code) \
{                               \
    SET_ERROR(error_code);      \
    LOG_ERROR()                 \
    return;                     \
}

#define ERROR_ASSERT(statement, error_code) \
    if ( !(statement) ) RAISE_ERROR(error_code);
