#ifdef ERROR_TYPE_

#define ERROR_BIT_ ((ERROR_TYPE_)1)

#define IS_OTHER_ERROR(other, error_code) \
    (((other)->error & (ERROR_BIT_ << (error_code))) != 0)

#define SET_OTHER_ERROR(other, error_code) \
    (other)->error |= ERROR_BIT_ << (error_code)

#define RESET_OTHER_ERROR(other, other_error_type, error_code) \
    (other)->error &= (other_error_type)(~(ERROR_BIT_ << (error_code)))

#define UPDATE_OTHER_ERROR_VALUE(other, other_error_type, error_code, statement) \
do { \
    if (statement) SET_OTHER_ERROR(other, error_code); \
    else RESET_OTHER_ERROR(other, other_error_type, error_code); \
} while(0)


#ifdef ERROR_SOURCE_

// Before including this header you MUST define:
// `ERROR_SOURCE_` - name of variable containing struct with error field
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


// TODO run destructor after error
#define START_PRINT_ERROR_FUNCTION \
static void FPrintError(FILE *const file, const ERROR_SOURCE_TYPE_ ERROR_SOURCE_) { \
    if (ERROR_SOURCE_ == NULL) { \
        fprintf(stderr, STR(ERROR_SOURCE_) "_IS_NULL_ERROR\n"); \
        return; \
    } \
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

#define END_PRINT_ERROR_FUNCTION \
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

#define SET_ERROR(error_code) SET_OTHER_ERROR(ERROR_SOURCE_, error_code)
#define RESET_ERROR(error_code) RESET_OTHER_ERROR(ERROR_SOURCE_, ERROR_TYPE_, error_code)
#define UPDATE_ERROR_VALUE(error_code, statement) UPDATE_OTHER_ERROR_VALUE(ERROR_SOURCE_, ERROR_TYPE_, error_code, statement)
#define IS_ERROR(error_code) IS_OTHER_ERROR(ERROR_SOURCE_, error_code)

#define LOG_ERROR() \
do { \
    fprintf(stderr, "Error occured in function %s in %s:%d\n", __func__, __FILE__, __LINE__); \
    FPrintError(stderr, ERROR_SOURCE_); \
} while (0)

#define RAISE_ERROR(error_code) \
do {                            \
    SET_ERROR(error_code);      \
    LOG_ERROR();                \
    return;                     \
} while (0)

#define ERROR_ASSERT(statement, error_code) \
    if ( !(statement) ) RAISE_ERROR(error_code)


#endif
#endif
